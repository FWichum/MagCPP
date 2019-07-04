#include "serialportcontroller.h"


serialportcontroller::serialportcontroller(std::queue<float> serialWriteQueue, std::queue<float> serialReadQueue)
{
    this->serialWriteQueue = serialWriteQueue;
    this->serialReadQueue = serialReadQueue;
}

void serialportcontroller::run()
{
    double writetimeout = 0.3; //waitForBytesWritten(300);  TODO
    while (true) {
        //message, reply, readBytes = self._serialWriteQueue.get() //  TODO
        // If the first part of the message is None this signals the process to close the port and stop
        float message = this->serialWriteQueue.front();
        // If the first part of the message is None this signals the process to close the port and stop
        if(message == 0) { // TODO
            break;
        }
        // If the first part of the message is a 1 this signals the process to trigger a quick fire using the RTS pin
        if(message == 1) {
            // TODO this->port.setRequestToSend(true);
        }
        // If the first part of the message is a -1 this signals the process to reset the RTS pin
        else if( message == -1 ){
            //TODO this->port.setRequestToSend(false);
        }
        // Otherwise, the message is a command string
        else {
            // There shouldn't be any rubbish in the input buffer, but check and clear it just in case
            /*
            if() {
                port.clear(QSerialPort::AllDirections);
            }
            */

        }
    }
    return;
}
