#ifndef MAGSTIM_H
#define MAGSTIM_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "connectionrobot.h"
#include "serialportcontroller.h"

#include <map>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <queue>
#include <tuple>
#include <list>

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QMetaType>
#include <QEventLoop>

//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

typedef std::tuple<QByteArray, QString, int> sendInfo;
typedef std::tuple<int, QByteArray> reciveInfo;

//=============================================================================================================
/**
* The base Magstim class. This is used for controlling 200^2 Magstim units, and acts as a parent class for
* the BiStim^2 and Rapid^2 sub-classes. It also creates two additional processes; one for the purposes
* of directly controlling the serial port and another for maintaining constant contact with the Magstim.
*   N.B. This class can effect limited control over BiStim^2 and Rapid^2 units, however some functionality
*   will not be able to be accessed and return values (including confirmation of commands) may be invalid.
*   To begin sending commands to the Magstim, and start the additional processes, you must first call connect().
*
* @brief The base Magstim class is used for controlling the unit.
*/

class MagStim : public QObject
{
    Q_OBJECT

public:
    //=========================================================================================================
    /**
    * Constructs a MagStim
    *
    * @param[in] serialConnection           The serial port
    * @param[in] parent                     Parent Qobject
    */
    MagStim(QString serialConnection, QObject* parent = 0);


    //FW: TODO Destruktor!?


    //=========================================================================================================
    /**
    * Connect to the Magstim.
    * This starts the serial port controller, as well as a process that constantly keeps in
    * contact with the Magstim so as not to lose control.
    *
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void connect(int &error);

    //=========================================================================================================
    /**
    * Disconnect from the Magstim.
    * This stops maintaining contact with the Magstim and turns the serial port controller off.
    *
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void disconnect(int &error);

    //=========================================================================================================
    /**
    * Enable/Disable remote control of stimulator. Disabling remote control will first disarm the Magstim unit.
    *
    * @param[in] enable                     whether to enable (True) or disable (False) control
    * @param[in] message                    [opt] TODO Doxygen
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void remoteControl(bool enable, std::map<QString, std::map<QString, double> > &message, int &error);

    //=========================================================================================================
    /**
    * Arm the stimulator
    *   N.B. You must allow at around 1 s for the stimulator to arm.
    *   If you send an arm() command when the Magstim is already armed, you will receive an non-fatal error
    *   reply from the Magstim that the command conflicts with the current settings.
    *   If the unit does not fire for more than 1 min while armed, it will disarm
    *
    * @param[in] delay                      [opt] enforce delay to allow Magstim time to arm (defaults to False)
    * @param[in] message                    [opt] TODO Doxygen
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void arm(bool delay, std::map<QString, std::map<QString, double> > &message, int &error);

    //=========================================================================================================
    /**
    * Disarm the stimulator.
    *
    * @param[in] message                    [opt] TODO Doxygen
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void disarm(std::map<QString, std::map<QString, double> > &message, int &error);

    //=========================================================================================================
    /**
    * Request current parameter settings from the Magstim.
    *
    * @param[in] error                      [opt] Chatches error code
    * @param[out] message                   TODO Doxygen
    */
    virtual std::map<QString, std::map<QString, double> > getParameters(int &error);

    //=========================================================================================================
    /**
    * Request current coil temperature from the Magstim.
    *   N.B. Coil1 and Coil2 refer to the separate windings in a single figure-8 coil connected to the Magstim.
    *   Magstim units will automatically disarm (and cannot be armed) if the coil temperature exceeds 40 degrees celsius.
    *
    * @param[in] error                      [opt] Chatches error code
    * @param[out] message                   TODO Doxygen
    */
    virtual std::map<QString, std::map<QString, double> > getTemperature(int &error);

    //=========================================================================================================
    /**
    * Helper function that returns True if the Magstim is armed or ready, False if not or if it could not be determined.
    *
    * @param[out] message                   TODO Doxygen
    */
    virtual bool isArmed();

    //=========================================================================================================
    /**
    * Helper function that returns True if the Magstim is under remote control, False if not or if it could not be determined.
    *
    * @param[out] message                   TODO Doxygen
    */
    virtual bool isUnderControl();

    //=========================================================================================================
    /**
    * Helper function that returns True if the Magstim is ready to fire, False if not or if it could not be determined.
    *
    * @param[out] message                   TODO Doxygen
    */
    virtual bool isReadyToFire();

    //=========================================================================================================
    /**
    * Set power level for Magstim.
    *   N.B. Allow 100 ms per unit drop in power, or 10 ms per unit increase in power.
    *
    * @param[in] newPower                   new power level (0-100)
    * @param[in] delay                      [opt] enforce delay to allow Magstim time to change Power (defaults to False)
    * @param[in] error                      [opt] Chatches error code
    * @param[in] commandByte                [opt] should not be changed by the user
    * @param[in] message                    [opt] TODO Doxygen
    */
    virtual void setPower(int newPower, bool delay, int &error, QString commandByte, std::map<QString, std::map<QString, double> > &message);

    //=========================================================================================================
    /**
    * 'Poke' the stimulator with an enable remote control command (only if currently connected).
    * This should be used prior to any time-senstive commands, such as triggering the magstim to coincide with
    * stimulus presentation. Conservatively, around 40-50ms should be enough time to allow for (~20ms if 'silently' poking).
    * This needs to be done to ensure that the ongoing communication with the magstim to maintain remote control
    * does not interfere with the sent command. Note that this simply resets the timer controlling this ongoing
    * communication (i.e., incrementing it a further 500 ms).
    *
    * @param[in] silent                     [opt] whether to bump polling robot but without sending enable remote control command (defaults to False)
    */
    virtual void poke();

    //=========================================================================================================
    /**
    * Fire the stimulator.
    *   N.B. Will only succeed if previously armed.
    *
    * @param[in] message                    [opt] TODO Doxygen
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void fire(std::map<QString, std::map<QString, double> > &message, int &error);

    //=========================================================================================================
    /**
    * Trigger the stimulator to fire with very low latency using the RTS pin and a custom serial connection.
    */
    virtual void quickFire();

    //=========================================================================================================
    /**
    * Reset the RTS pin used for quick firing.
    *   N.B. There must be a few ms between triggering QuickFire and reseting the pin.
    */
    virtual void resetQuickFire();

    //=========================================================================================================
    /**
    * Error Codes
    * TODO Doxygen
    */
    const int INVALID_COMMAND_ERR       = 3; // INVALID_COMMAND_ERR: Invalid command sent.
    const int INVALID_DATA_ERR          = 4; // INVALID_DATA_ERR: Invalid data provided.
    const int COMMAND_CONFLICT_ERR      = 5; // COMMAND_CONFLICT_ERR: Command conflicts with current system configuration.
    const int INVALID_CONFIRMATION_ERR  = 6; // INVALID_CONFIRMATION_ERR: Unexpected command confirmation received.
    const int CRC_MISMATCH_ERR          = 7; // CRC_MISMATCH_ERR: Message contents and CRC value do not match.
    const int NO_REMOTE_CONTROL_ERR     = 8; // NO_REMOTE_CONTROL_ERR: You have not established control of the Magstim unit.
    const int PARAMETER_ACQUISTION_ERR  = 9; // PARAMETER_ACQUISTION_ERR: Could not obtain prior parameter settings.
    const int PARAMETER_UPDATE_ERR      = 10;// PARAMETER_UPDATE_ERR: Could not update secondary parameter to accommodate primary parameter change.
    const int PARAMETER_FLOAT_ERR       = 11;// PARAMETER_FLOAT_ERR: A float value is not allowed for this parameter.
    const int PARAMETER_PRECISION_ERR   = 12;// PARAMETER_PRECISION_ERR: Only one decimal placed allowed for this parameter.
    const int PARAMETER_RANGE_ERR       = 13;// PARAMETER_RANGE_ERR: Parameter value is outside the allowed range.
    const int GET_SYSTEM_STATUS_ERR     = 14;// GET_SYSTEM_STATUS_ERR: Cannot call getSystemStatus() until software version has been established.
    const int SYSTEM_STATUS_VERSION_ERR = 15;// SYSTEM_STATUS_VERSION_ERR: Method getSystemStatus() is not compatible with your software version.
    const int SEQUENCE_VALIDATION_ERR   = 16;// SEQUENCE_VALIDATION_ERR: You must call validateSequence() before you can run a rTMS train.
    const int MIN_WAIT_TIME_ERR         = 17;// MIN_WAIT_TIME_ERR: Minimum wait time between trains violated. Call isReadyToFire() to check.
    const int MAX_ON_TIME_ERR           = 18;// MAX_ON_TIME_ERR: Maximum on time exceeded for current train.


protected:
    //=========================================================================================================
    /**
    * Interprets responses sent from the Magstim unit.
    *
    * @param[in] responseString             TODO Doxygen
    * @param[in] responseType               TODO Doxygen
    *
    * @return TODO Doxygen
    */
    virtual std::map<QString, std::map<QString, double>> parseMagstimResponse(std::list<int> responseString, QString responseType);

    //=========================================================================================================
    /**
    * Interprets responses sent from the Magstim unit.
    *
    * @param[in] responseString             TODO Doxygen
    *
    * @return version of Magstim unit (X, Y, Z)
    */
    virtual std::tuple<int, int, int> parseMagstimResponse_version(std::list<int> responseString);

    //=========================================================================================================
    /**
    * TODO Doxygen
    *
    * @param[in] serialConnection           TODO Doxygen
    */
    virtual void setupSerialPort(QString serialConnection);

    //=========================================================================================================
    /**
    * Overloaded function. Process Magstim command.
    *
    * @param[in] commandString              command and data characters making up the command string (N.B. do not include CRC character)
    * @param[in] receiptType                determines the how the response will be processed
    * @param[in] readBytes                  number of bytes in the response
    * @param[in] message                    TODO Doxygen
    */
    virtual int processCommand(QString commandString, QString receiptType, int readBytes, std::map<QString, std::map<QString, double> > &message);

    //=========================================================================================================
    /**
    * Overloaded function. Process Magstim command.
    *
    * @param[in] commandString              command and data characters making up the command string (N.B. do not include CRC character)
    * @param[in] receiptType                determines the how the response will be processed
    * @param[in] readBytes                  number of bytes in the response
    * @param[in] version                    version of Magstim unit (X, Y, Z)
    */
    virtual int processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version);

    //=========================================================================================================
    /**
    * Overloaded function. Process Magstim command.
    *
    * @param[in] commandString              command and data characters making up the command string (N.B. do not include CRC character)
    * @param[in] receiptType                determines the how the response will be processed
    * @param[in] readBytes                  number of bytes in the response
    * @param[in] version                    version of Magstim unit (X, Y, Z)
    * @param[in] message                    TODO Doxygen
    */
    virtual int processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version, std::map<QString, std::map<QString, double> > &message);

    //=========================================================================================================
    /**
    * Calculates checksum for the command string
    *
    * @param[in] command                    TODO Doxygen
    *
    * @return the CRC checksum
    */
    virtual char calcCRC(QByteArray command);

    static int er;                                                    /**< Use as placeholder */
    static std::tuple<int, int, int> ver;                             /**< Use as placeholder */
    static std::map<QString, std::map<QString, double>> mes;          /**< Use as placeholder */

    std::queue<std::tuple<QByteArray, QString, int>> m_sendQueue;     /**< Queue for writing to Magstim */
    std::queue<std::tuple<int, QByteArray>> m_receiveQueue;           /**< Queue for reading from Magstim */
    std::queue<float> m_robotQueue;                                   /**< Queue for controlling the connectionRobot */

    SerialPortController *m_connection;                               /**< Controls the serial port */
    ConnectionRobot *m_robot;                                         /**< Keeps remote control */

    QEventLoop m_loop;                                                /**< TODO Doxygen */
    bool m_connected;                                                 /**< TODO Doxygen */
    std::tuple<QByteArray, QString, int> m_connectionCommand;         /**< TODO Doxygen */
    int m_parameterReturnByte;                                        /**< TODO Doxygen */

public slots:
    //=========================================================================================================
    /**
    * Updates the recive queue after reading something from Magstim. Emits a signal to allow leaving QEventLoop.
    *
    * @param[in] info                       TODO Doxygen
    */
    void updateReceiveQueue(reciveInfo info);

signals:
    //=========================================================================================================
    /**
    * Updates the send queue. A message will be sent to Magstim unit via SerialPortController.
    */
    void updateSendQueue(const sendInfo &info);

    //=========================================================================================================
    /**
    * TODO Doxygen
    */
    void updateRobotQueue(const float &info);

    //=========================================================================================================
    /**
    * A message from the Magstim unit was read. Allows to leave QEventLoop.
    */
    void readInfo();
};

Q_DECLARE_METATYPE(std::tuple<>);
#endif // MAGSTIM_H
