#include "connectionrobot.h"

#include <cmath>

connectionRobot::connectionRobot(std::queue<float> serialWriteQueue, std::queue<float> updateRobotQueue)
{
    this->serialWriteQueue = serialWriteQueue;
    this->updateRobotQueue = updateRobotQueue;
}

void connectionRobot::run()
{

}

clock_t connectionRobot::defaultTimer()
{

}

void connectionRobot::setCommand()
{

}
