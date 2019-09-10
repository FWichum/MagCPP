#include "connectionrobot.h"
#include <cmath>


ConnectionRobot::ConnectionRobot(std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue, std::queue<float> updateRobotQueue)
{
    this->m_serialWriteQueue = serialWriteQueue;
    this->m_updateRobotQueue = updateRobotQueue;
    this->m_stopped = false;
    this->m_paused = true;
    this->m_nextPokeTime = NAN;
    this->m_connectionCommand = std::make_tuple("","",0); //FW: TODO
//    this->moveToThread(this);
}


//*************************************************************************************************************

void ConnectionRobot::run()
{
//    this->exec();
    // This sends an "enable remote control" command to the serial port controller every 500ms (if armed) or 5000 ms (if disarmed); only runs once the stimulator is armed
    double pokeLatency = 5;
    while (true) {
        // This locker will lock the mutex until it is destroyed, i.e. when one while loop is over
        QMutexLocker locker(&m_mutex);
        locker.relock();

        // If the robot is currently paused, wait until we get a None (stop) or a non-negative number (start/resume) in the queue
        while (this->m_paused) {
            float message = this->m_updateRobotQueue.front(); // FW: TODO ist float der richtige Typ? Mit int ist NAN nicht möglich :/
            this->m_updateRobotQueue.pop();
            if (std::isnan(message)) {  // FW: TODO eventuell
                this->m_stopped = true;
                this->m_paused = false;
            } else if (message >= 0) {
                // If message is a 2, that means we've just armed so speed up the poke latency (not sure that's possible while paused, but just in case)
                if ((int) message == 2) {
                    pokeLatency = 0.5;
                    // If message is a 1, that means we've just disarmed so slow down the poke latency
                } else if ((int) message == 1) {
                    pokeLatency = 5;
                }
                this->m_paused = false;
            }
        }
        // Check if we're stopping the robot
        if (this->m_stopped) {
            break;
        }
        // Update next poll time to the next poke latency
        this->m_nextPokeTime = defaultTimer() + pokeLatency;
        // While waiting for next poll...
        if (defaultTimer() >= this->m_nextPokeTime) {
            // this->m_serialWriteQueue.push(this->m_connectionCommand); // FW: TODO is this needed?
            emit this->updateSerialWriteQueue(this->m_connectionCommand);
        } else do { // FW: C++ Version of While-Else:
            // ...check to see if there has been an update send from the parent magstim object
            if (!this->m_updateRobotQueue.empty()) {
                float message = this->m_updateRobotQueue.front();
                this->m_updateRobotQueue.pop();
                // If the message is None this signals the process to stop
                if (std::isnan(message)) {  // FW: TODO eventuell
                    this->m_stopped = true;
                    break;
                    //  If the message is -1, we've relinquished remote control so signal the process to pause
                } else if ((int) message == -1) {
                    pokeLatency = 5;
                    this->m_paused = true;
                    break;
                    // Any other message signals a command has been sent to the serial port controller
                } else {
                    // If message is a 2, that means we've just armed so speed up the poke latency (not sure that's possible while paused, but just in case)
                    if ((int) message == 2) {
                        pokeLatency = 0.5;
                        // If message is a 1, that means we've just disarmed so slow down the poke latency
                    } else if ((int) message == 1) {
                        pokeLatency = 5;
                    }
                    this->m_nextPokeTime = defaultTimer() + pokeLatency;
                }
            }
        } while (defaultTimer() < this->m_nextPokeTime);
        locker.unlock();
    }
    return;
}


//*************************************************************************************************************

clock_t ConnectionRobot::defaultTimer()
{
    // FW: TODO switch for each System !?
    return clock();
}


//*************************************************************************************************************

void ConnectionRobot::setCommand(std::tuple<QByteArray, QString, int> connectionCommand)
{
    // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
    QMutexLocker locker(&m_mutex);
    locker.relock();
    this->m_connectionCommand = connectionCommand;
    locker.unlock();
}


//*************************************************************************************************************

void ConnectionRobot::updateUpdateRobotQueue(const float info)
{
    // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
    QMutexLocker locker(&m_mutex);
    locker.relock();
    this->m_updateRobotQueue.push(info);
    locker.unlock();
}
