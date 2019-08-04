#ifndef CONNECTIONROBOT_H
#define CONNECTIONROBOT_H

#include <queue>
#include <ctime>
#include <map>
#include <QByteArray>
#include <QString>

class connectionRobot
{
public:
    connectionRobot(std::queue<std::map<QByteArray, QString, int>> serialWriteQueue, std::queue<float> updateRobotQueue);
    void run();
    clock_t defaultTimer();
    void setCommand(); //FW: TODO which Type? What is this?

private:
    std::queue<std::map<QByteArray, QString, int>> serialWriteQueue;
    std::queue<float> updateRobotQueue;
    bool stopped;
    bool paused;
    double nextPokeTime;
    // FW: TODO connectionCommand
};

#endif // CONNECTIONROBOT_H
