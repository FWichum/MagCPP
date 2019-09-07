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

#include "rs232.h"

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QMetaType>
#include <QEventLoop>



typedef std::tuple<QByteArray, QString, int> sendInfo;
typedef std::tuple<int, QByteArray> reciveInfo;

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
    virtual void remoteControl(bool enable, std::map<QString, std::map<QString, int> > &message, int &error);

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
    virtual void arm(bool delay, std::map<QString, std::map<QString, int> > &message, int &error);

    //=========================================================================================================
    /**
    * Disarm the stimulator.
    *
    * @param[in] message                    [opt] TODO Doxygen
    * @param[in] error                      [opt] Chatches error code
    */
    virtual void disarm(std::map<QString, std::map<QString, int> > &message, int &error);

    //=========================================================================================================
    /**
    * Request current parameter settings from the Magstim.
    *
    * @param[in] error                      [opt] Chatches error code
    * @param[out] message                   TODO Doxygen
    */
    virtual std::map<QString, std::map<QString, int> > getParameters(int &error);

    //=========================================================================================================
    /**
    * Request current coil temperature from the Magstim.
    *   N.B. Coil1 and Coil2 refer to the separate windings in a single figure-8 coil connected to the Magstim.
    *   Magstim units will automatically disarm (and cannot be armed) if the coil temperature exceeds 40 degrees celsius.
    *
    * @param[in] error                      [opt] Chatches error code
    * @param[out] message                   TODO Doxygen
    */
    virtual std::map<QString, std::map<QString, int> > getTemperature(int &error);

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
    virtual void setPower(int newPower, bool delay, int &error, QString commandByte, std::map<QString, std::map<QString, int> > &message);

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
    virtual void fire(std::map<QString, std::map<QString, int> > &message, int &error);

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
    virtual std::map<QString, std::map<QString, int>> parseMagstimResponse(std::list<int> responseString, QString responseType);
    virtual std::tuple<int, int, int> parseMagstimResponse_version(std::list<int> responseString);
    virtual void setupSerialPort(QString serialConnection);
    virtual int processCommand(QString commandString, QString receiptType, int readBytes, std::map<QString, std::map<QString, int> > &message);
    virtual int processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version);
    virtual int processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version, std::map<QString, std::map<QString, int> > &message);
    virtual char calcCRC(QByteArray command);

    static int er;
    static std::tuple<int, int, int> ver;
    static std::map<QString, std::map<QString, int>> mes;

    std::queue<std::tuple<QByteArray, QString, int>> sendQueue;
    std::queue<std::tuple<int, QByteArray>> receiveQueue;
    std::queue<float> robotQueue;
    ConnectionRobot *robot;
    SerialPortController *connection;
    QEventLoop loop;
    bool connected;
    std::tuple<QByteArray, QString, int> connectionCommand;
    int parameterReturnByte;
    // connection.daemon = true; //FW: TODO
    // ConnectionRobot robot;
    // robot.daemon = true; //FW: TODO
    // auto queryCommand; //FW: TODO

public slots:
    void updateReceiveQueue(reciveInfo info);

signals:
    void updateSendQueue(const sendInfo &info);
    void updateRobotQueue(const float &info);
    void readInfo();
};

Q_DECLARE_METATYPE(std::tuple<>);
#endif // MAGSTIM_H
