#ifndef CONNECTIONROBOT_H
#define CONNECTIONROBOT_H

#include <queue>
#include <ctime>
#include <tuple>
#include <QByteArray>
#include <QString>
#include <QThread>

class connectionRobot : QThread
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
};

#endif // CONNECTIONROBOT_H
