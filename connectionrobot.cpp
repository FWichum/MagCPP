#include "connectionrobot.h"

#include <cmath>

connectionRobot::connectionRobot(std::queue<float> serialWriteQueue, std::queue<float> updateRobotQueue)
{
    this->serialWriteQueue = serialWriteQueue;
    this->updateRobotQueue = updateRobotQueue;
    this->stopped = false;
    this->paused = true;
    this->nextPokeTime = NAN;
    // this->connectionCommand = NAN; //FW: TODO
}

void connectionRobot::run()
{
    // This sends an "enable remote control" command to the serial port controller every 500ms (if armed) or 5000 ms (if disarmed); only runs once the stimulator is armed
    double pokeLatency = 5;
    while (true) {
        // If the robot is currently paused, wait until we get a None (stop) or a non-negative number (start/resume) in the queue
        while (this->paused) {
            float message = this->updateRobotQueue.front(); // FW: TODO ist float der richtige Typ? Mit int ist NAN nicht möglich :/
            this->updateRobotQueue.pop();
            if (std::isnan(message)) {  // FW: TODO eventuell
                this->stopped = true;
                this->paused = false;
            } else if (message >= 0) {
                int message_int = (int) message;    // FW: TODO neuen Variablennamen benutzen!? notwendig?
                // If message is a 2, that means we've just armed so speed up the poke latency (not sure that's possible while paused, but just in case)
                if (message_int == 2) {
                    pokeLatency = 0.5;
                    // If message is a 1, that means we've just disarmed so slow down the poke latency
                } else if (message_int == 1) {
                    pokeLatency = 5;
                }
                this->paused = false;
            }
        }
        // Check if we're stopping the robot
        if (this->stopped) {
            break;
        }
        // Update next poll time to the next poke latency
        this->nextPokeTime = defaultTimer() + pokeLatency;
        // While waiting for next poll...
        while (defaultTimer() < this->nextPokeTime) {
            // ...check to see if there has been an update send from the parent magstim object
            if (!this->updateRobotQueue.empty()) {
                float message = this->updateRobotQueue.front(); // FW: TODO ist float der richtige Typ? Mit int ist NAN nicht möglich :/
                this->updateRobotQueue.pop();
                // If the message is None this signals the process to stop
                if (std::isnan(message)) {  // FW: TODO eventuell
                    this->stopped = true;
                    break;
                    //  If the message is -1, we've relinquished remote control so signal the process to pause
                } else if (message == -1) { // FW: TODO Float Gefahr!!! float2int
                    pokeLatency = 5;
                    this->paused = true;
                    break;
                    // Any other message signals a command has been sent to the serial port controller
                } else {
                    // If message is a 2, that means we've just armed so speed up the poke latency (not sure that's possible while paused, but just in case)
                    if (message == 2) { // FW: TODO Float Gefahr!!! float2int
                        pokeLatency = 0.5;
                        // If message is a 1, that means we've just disarmed so slow down the poke latency
                    } else if (message == 1) { // FW: TODO Float Gefahr!!! float2int
                        pokeLatency = 5;
                    }
                    this->nextPokeTime = defaultTimer() + pokeLatency;
                }
            }
        } // FW: TODO while else

    }
    return;
}

clock_t connectionRobot::defaultTimer()
{
    // FW: TODO switch for each System !?
    return clock();
}

void connectionRobot::setCommand()
{
    // FW: TODO
}
