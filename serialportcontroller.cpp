#include "serialportcontroller.h"
#include <qiodevice.h>

#include <QIODevice>


serialportcontroller::serialportcontroller(QString serialConnection, std::queue<float> serialWriteQueue, std::queue<float> serialReadQueue)
{
    this->serialWriteQueue = serialWriteQueue;
    this->serialReadQueue = serialReadQueue;
    this->address = serialConnection;
}

void serialportcontroller::run()
{
    /*
    Continuously monitor the serialWriteQueue for commands from other Python processes to be sent to the Magstim.
    When requested, will return the automated reply from the Magstim unit to the calling process via the serialReadQueue.
    N.B. This should be called via start() from the parent Python process.
    */

    // N.B. most of these settings are actually the default in PySerial, but just being careful.
    this->port.setPortName(this->address);
    this->port.setBaudRate(QSerialPort::Baud9600);
    this->port.setDataBits(QSerialPort::Data8);
    this->port.setStopBits(QSerialPort::OneStop);
    this->port.setParity(QSerialPort::NoParity);
    this->port.setFlowControl(QSerialPort::NoFlowControl);

    // Make sure the RTS pin is set to off
    this->port.setRequestToSend(false);

    //double writetimeout = 0.3; //waitForBytesWritten(300);  TODO
    this->port.waitForBytesWritten(300);

    while (true) {
        //message, reply, readBytes = self._serialWriteQueue.get() //  TODO
        // If the first part of the message is None this signals the process to close the port and stop
        float message = this->serialWriteQueue.front();     // TODO
        // If the first part of the message is None this signals the process to close the port and stop
        if(message == 0) { // TODO
            break;
        }
        //TODO  was für Werte gibt es in Message?
        // If the first part of the message is a 1 this signals the process to trigger a quick fire using the RTS pin
        if(message == 1) {
            this->port.setRequestToSend(true);
        }
        // If the first part of the message is a -1 this signals the process to reset the RTS pin
        else if( message == -1 ){
            this->port.setRequestToSend(false);
        }
        // Otherwise, the message is a command string
        else {
            // There shouldn't be any rubbish in the input buffer, but check and clear it just in case
            // if(this->port.readBufferSize()!= 0) {           // funktioniert das überhaupt so? wird es benötigt?
                this->port.clear(QSerialPort::AllDirections);

            try {
                this->port.write("message");
                try {
                    this->port.read(1);
                } catch (...) {
                    this->serialReadQueue.push(1);
                }
            } catch (...) {
                this->serialReadQueue.push(1);
            }




        }
    }


    return;
}
