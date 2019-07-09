#ifndef MAGSTIM_H
#define MAGSTIM_H


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
    std::map<QString, std::map<QString, int>> parseMagstimResponse(std::list<QByteArray> responseString, QString responseType);
    void connect();
    void disconnect();
    void remoteControl();
    void getParameters();
    void setPower();
    void getTemperature();
    void poke();
    void arm();
    void disarm();
    void isArmed();
    void isUnderControl();
    void isReadyToFire();
    void fire();
    void resetQuickFire();
    void quickFire();



private:
    void setupSerialPort(QString serialConnection);
    std::tuple<int, std::map<QString, std::map<QString, int>>> processCommand(QString commandString, QString receiptType, int readBytes);
    QByteArray calcCRC(QString command);

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

};

#endif // MAGSTIM_H
