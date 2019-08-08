#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

#include <QtSerialPort/QSerialPort>
#include <queue>
#include <QThread>
#include <QByteArray>
#include <QMutexLocker>
#include <QMutex>


class serialPortController : public QThread
{
    Q_OBJECT

public:
    serialPortController(QString serialConnection,
                         std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue,
                         std::queue<std::tuple<int, QByteArray>> serialReadQueue);
    void run() override;

private:
    QSerialPort port;
    std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue;
    std::queue<std::tuple<int, QByteArray>> serialReadQueue;
    QString address;
    QMutex mutex;

    const int SERIAL_WRITE_ERROR = 1; // SERIAL_WRITE_ERR: Could not send the command.
    const int SERIAL_READ_ERROR  = 2; // SERIAL_READ_ERR:  Could not read the magstim response.

public slots:
    void updateSerialWriteQueue(std::tuple<QByteArray, QString, int> info);

signals:
    void updateSerialReadQueue(const std::tuple<int, QByteArray> &info);
};

#endif // SERIALPORTCONTROLLER_H
