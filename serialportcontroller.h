#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

#include <QtSerialPort/QSerialPort>
#include <queue>
#include <QThread>
#include <QByteArray>

class serialPortController : public QThread
{
    Q_OBJECT

public:
    serialPortController(QString serialConnection, std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue, std::queue<float> serialReadQueue);
    void run() override;

private:
    QSerialPort port;
    std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue;
    std::queue<float> serialReadQueue;
    QString address;

    const int SERIAL_WRITE_ERROR = 1; // SERIAL_WRITE_ERR: Could not send the command.
    const int SERIAL_READ_ERROR  = 2; // SERIAL_READ_ERR:  Could not read the magstim response.

signals:
    void resultReady(const QString &s);
};

#endif // SERIALPORTCONTROLLER_H
