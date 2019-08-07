#ifndef CONNECTIONROBOT_H
#define CONNECTIONROBOT_H

#include <queue>
#include <ctime>
#include <tuple>
#include <QByteArray>
#include <QString>
#include <QThread>

class connectionRobot : public QThread
{
     Q_OBJECT

public:
    connectionRobot(std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue, std::queue<float> updateRobotQueue);
    void run() override;
    clock_t defaultTimer();
    void setCommand(); //FW: TODO which Type? What is this?

private:
    std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue;
    std::queue<float> updateRobotQueue;
    bool stopped;
    bool paused;
    double nextPokeTime;
    // FW: TODO connectionCommand

public slots:
    void updateUpdateRobotQueue(const float info);
    void updateSerialWriteQueue(const std::tuple<QByteArray, QString, int> info);
};

#endif // CONNECTIONROBOT_H
