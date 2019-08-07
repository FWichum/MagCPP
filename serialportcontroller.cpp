#include "serialportcontroller.h"
#include <qiodevice.h>

#include <QIODevice>


serialPortController::serialPortController(QString serialConnection, std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue, std::queue<std::tuple<int, QByteArray> > serialReadQueue)
{
    this->serialWriteQueue = serialWriteQueue;
    this->serialReadQueue = serialReadQueue;
    this->address = serialConnection;
}

void serialPortController::run()
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

    this->port.waitForBytesWritten(300);

    while (true) {
        // If the first part of the message is None this signals the process to close the port and stop
        QByteArray bmessage = std::get<QByteArray>(this->serialWriteQueue.front());
        float message = bmessage.toFloat();
        QString reply = std::get<QString>(this->serialWriteQueue.front());
        int readBytes = std::get<int>(this->serialWriteQueue.front());
        this->serialWriteQueue.pop();
        char *c;
        // If the first part of the message is None this signals the process to close the port and stop
        if((int)message == 0) {
            break;
        }
        // If the first part of the message is a 1 this signals the process to trigger a quick fire using the RTS pin
        else if((int)message == 1) {
            this->port.setRequestToSend(true);
        }
        // If the first part of the message is a -1 this signals the process to reset the RTS pin
        else if((int)message == -1 ){
            this->port.setRequestToSend(false);
        }
        // Otherwise, the message is a command string
        else {
            // There shouldn't be any rubbish in the input buffer, but check and clear it just in case
            // if(this->port.readBufferSize()!= 0) {    // FIXME funktioniert das überhaupt so? wird es benötigt?
            this->port.clear(QSerialPort::AllDirections);
            try {
                // Try writing to the port
                this->port.write(bmessage);
                // Read response (this gets a little confusing, as I don't want to rely on timeout to know if there's an error)
                try {
                    this->port.read(c,1);
                    bmessage.append(c);
                    if (bmessage.at(0) == 'N') {
                        while((int) bmessage.back() > 0) {
                            this->port.read(c,1);
                            bmessage.append(c);
                        }
                        // After the end of the version number, read one more byte to grab the CRC
                        this->port.read(c,1);
                        bmessage.append(c);
                        // If the first byte is not '?', then the message was understood
                        // so carry on reading in the response (if it was a '?', then this will be the only returned byte).
                    } else if (bmessage.at(0) != '?') {
                        // Read the second byte
                        this->port.read(c,1);
                        bmessage.append(c);
                        // If the second returned byte is a '?' or 'S', then the data value supplied either wasn't acceptable ('?') or the command conflicted with the current settings ('S'),
                        // In these cases, just grab the CRC - otherwise, everything is ok so carry on reading the rest of the message
                        if (bmessage.at(bmessage.length()-1) != '?' && bmessage.at(bmessage.length()-1) != 'N') {
                            this->port.read(c,readBytes-2);
                            bmessage.append(c);
                        } else {
                            this->port.read(c,1);
                            bmessage.append(c);
                        }
                        if (reply.toInt()) {
                            this->serialReadQueue.push(std::make_tuple(0, bmessage)); //FW: TODO need this?
                            emit updateSerialReadQueue(std::make_tuple(0, bmessage));
                        }
                    }
                } catch (...) { // FW: FIXME
                    this->serialReadQueue.push(std::make_tuple(serialPortController::SERIAL_READ_ERROR, bmessage)); //FW: TODO need this?
                    emit updateSerialReadQueue(std::make_tuple(serialPortController::SERIAL_READ_ERROR, bmessage));
                }
            } catch (...) { //FW: FIXME
                this->serialReadQueue.push(std::make_tuple(serialPortController::SERIAL_WRITE_ERROR, bmessage)); //FW: TODO need this?
                emit updateSerialReadQueue(std::make_tuple(serialPortController::SERIAL_WRITE_ERROR, bmessage));
            }
        }
    }
    // If we get here, it's time to shutdown the serial port controller
    this->port.close();
    return;
}

void serialPortController::updateSerialWriteQueue(std::tuple<QByteArray, QString, int> info)
{
    this->serialWriteQueue.push(info);
}
