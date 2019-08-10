#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

#include <QtSerialPort/QSerialPort>
#include <queue>
#include <QThread>
#include <QByteArray>
#include <QMutexLocker>
#include <QMutex>

typedef std::tuple<QByteArray, QString, int> sendInfo;
typedef std::tuple<int, QByteArray> reciveInfo;
Q_DECLARE_METATYPE(sendInfo);
Q_DECLARE_METATYPE(reciveInfo);

class SerialPortController : public QThread
{
    Q_OBJECT

public:
    SerialPortController(QString serialConnection,
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
    void updateSerialWriteQueue(sendInfo info);

signals:
    void updateSerialReadQueue(const reciveInfo &info);
};

#endif // SERIALPORTCONTROLLER_H
