#ifndef CONNECTIONROBOT_H
#define CONNECTIONROBOT_H

#include <queue>
#include <ctime>
#include <tuple>
#include <QByteArray>
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>
#include "serialportcontroller.h"

typedef std::tuple<QByteArray, QString, int> sendInfo;
typedef std::tuple<int, QByteArray> reciveInfo;
//Q_DECLARE_METATYPE(sendInfo);
//Q_DECLARE_METATYPE(reciveInfo);

class ConnectionRobot : public QThread
{
     Q_OBJECT

public:
    // Note that all functions except for run, are run in the callers thread
    ConnectionRobot(std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue, std::queue<float> updateRobotQueue);
    void run() override;
    clock_t defaultTimer();
    void setCommand(std::tuple<QByteArray, QString, int> connectionCommand);

private:
    std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue;
    std::queue<float> updateRobotQueue;
    bool stopped;
    bool paused;
    double nextPokeTime;
    std::tuple<QByteArray, QString, int> connectionCommand;
    QMutex mutex;


public slots:
    void updateUpdateRobotQueue(const float info);

signals:
    void updateSerialWriteQueue(const sendInfo info);
};

#endif // CONNECTIONROBOT_H
