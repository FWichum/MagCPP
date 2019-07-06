#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

#include <QtSerialPort/QSerialPort>
#include <queue>
class serialportcontroller
{
public:
    serialportcontroller(QString serialConnection, std::queue<float> serialWriteQueue, std::queue<float> serialReadQueue);
    void run();
private:

    QSerialPort port;
    std::queue<float> serialWriteQueue;
    std::queue<float> serialReadQueue;
    std::string SERIAL_WRITE_ERROR;
    std::string SERIAL_READ_ERROR;
};

#endif // SERIALPORTCONTROLLER_H
