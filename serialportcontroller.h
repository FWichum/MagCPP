#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H
#include <QtSerialPort/QSerialPort>
#include <queue>

class serialPortController
{
public:
    serialPortController(std::queue<float> serialWriteQueue, std::queue<float> serialReadQueue);
    void run();

private:
    QSerialPort port;
       std::queue<float> serialWriteQueue;
       std::queue<float> serialReadQueue;
};

#endif // SERIALPORTCONTROLLER_H
