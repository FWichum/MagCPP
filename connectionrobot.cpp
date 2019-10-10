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
    // Quit when message arrived in Queue
    QObject::connect(this, &ConnectionRobot::readInfo,
                     &this->m_loop, &QEventLoop::quit, Qt::ConnectionType::DirectConnection);
    // Quit when countdown
    QObject::connect(&this->m_timer, &QTimer::timeout,
                     &this->m_loop, &QEventLoop::quit, Qt::ConnectionType::DirectConnection);
}


//*************************************************************************************************************

void ConnectionRobot::run()
{
    // This sends an "enable remote control" command to the serial port controller every 500ms (if armed) or 5000 ms (if disarmed); only runs once the stimulator is armed
    double pokeLatency = 5;

    while (true) {
        // This locker will lock the mutex until it is destroyed, i.e. when one while loop is over
        QMutexLocker locker(&m_mutex);

        // If the robot is currently paused, wait until we get a None (stop) or a non-negative number (start/resume) in the queue
        while (this->m_paused) {
            // wait for new entry in RobotQueue
//            m_loop.exec();
            if (!this->m_updateRobotQueue.empty()) {
                float message = this->m_updateRobotQueue.front();
                this->m_updateRobotQueue.pop();
                if (std::isnan(message)) {
                    this->m_stopped = true;
                    this->m_paused = false;
                } else if ((int) message >= 0) {
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
        }

        // Check if we're stopping the robot
        if (this->m_stopped) {
            break;
        }

        // Update next poll time to the next poke latency
        this->m_nextPokeTime = clock() + pokeLatency * CLOCKS_PER_SEC;

        // While waiting for next poll...
        bool interrupted = false;
        while (clock() < this->m_nextPokeTime) {
//            m_timer.start(pokeLatency*1000); // ms
//            m_loop.exec(); // stops when: (1) timer or (2) signal
            // ...check to see if there has been an update send from the parent magstim object
            if (!this->m_updateRobotQueue.empty()) {
                float message = this->m_updateRobotQueue.front();
                this->m_updateRobotQueue.pop();

                // If the message is None this signals the process to stop
                if (std::isnan(message)) {
                    this->m_stopped = true;
                    interrupted = true;
                    break;

                    //  If the message is -1, we've relinquished remote control so signal the process to pause
                } else if ((int) message == -1) {
                    pokeLatency = 5;
                    this->m_paused = true;
                    interrupted = true;
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
                    this->m_nextPokeTime = clock() + pokeLatency * CLOCKS_PER_SEC;
                }
            }
        }

        // Send message if not stopped or paused
        if (clock() >= this->m_nextPokeTime && ~interrupted) {
            emit this->updateSerialWriteQueue(this->m_connectionCommand);
        }

    }
    return;
}


//*************************************************************************************************************
// runs in the main thread so use is not safe

//clock_t ConnectionRobot::defaultTimer()
//{
//    // FW: TODO switch for each System !?
//    return clock();
//}


//*************************************************************************************************************

void ConnectionRobot::setCommand(std::tuple<QByteArray, QString, int> connectionCommand)
{
    // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
    QMutexLocker locker(&m_mutex);
    this->m_connectionCommand = connectionCommand;

}


//*************************************************************************************************************

void ConnectionRobot::updateUpdateRobotQueue(const float info)
{
    // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
//    QMutexLocker locker(&m_mutex);
    this->m_updateRobotQueue.push(info);
    emit readInfo();
}
