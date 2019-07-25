#ifndef MAGSTIM_H
#define MAGSTIM_H
#include <stdio.h>
#include <iostream>
#include "rs232.h"
#include <math.h>
#include <iomanip>
#include <sstream>


#include <QString>
#include <queue>
#include <tuple>
#include <list>
#include <map>
#include <QByteArray>
#include "connectionrobot.h"

class MagStim
{
public:
    MagStim(QString serialConnection);
    bool connect(std::string port);
    void disconnect();
    bool get_status();
    std::map<QString, std::map<QString, int>> parseMagstimResponse(std::list<int> responseString, QString responseType);
    std::tuple<int, int, int> parseMagstimResponse_version(std::list<int> responseString);
    void remoteControl(bool enable, std::map<QString, std::map<QString, int> > &message, int &error);
    std::map<QString, std::map<QString, int> > getParameters();
    void setPower();
    std::tuple<int, std::map<QString, std::map<QString, int>>> getTemperature();
    void poke();
    void arm();
    void disarm(std::map<QString, std::map<QString, int> > &message, int &error);
    void isArmed();
    void isUnderControl();
    void isReadyToFire();
    std::tuple<int, std::map<QString, std::map<QString, int>>> fire(bool receipt=false);
    void resetQuickFire();
    void quickFire();

    // ErrorCodes
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


private:
    void setupSerialPort(QString serialConnection);
    int processCommand(QString commandString, QString receiptType, int readBytes, std::map<QString, std::map<QString, int> > &message);
    int processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version);
    int processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version, std::map<QString, std::map<QString, int> > &message);
    char calcCRC(QByteArray command);

    static int er;
    static std::tuple<int, int, int> ver;
    static std::map<QString, std::map<QString, int>> mes;

    std::queue<float> sendQueue;
    std::queue<float> receiveQueue;
    std::queue<float> robotQueue;
    // connection.daemon = true; //FW: TODO
    connectionRobot robot;
    // robot.daemon = true; //FW: TODO
    bool connected;
    // connectionCommand = (b'Q@n', None, 3) //FW: TODO
    // auto queryCommand; //FW: TODO
    int parameterReturnByte;

    int bdrate=19200;
    bool encode_command(uint8_t *destination, uint8_t *data);
    int cp_num=16;
    uint8_t command[13]={};
    char mode[4]={'8','N','1',0};
    uint8_t stat_command[10]={0x40,0x30,0x30,0x4D,0x30,0x30,0x34,0x44,0x0D,0x0A}; // stimmt noch nicht
};

#endif // MAGSTIM_H
