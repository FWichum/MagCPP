#include "serialportcontroller.h"
#include <qiodevice.h>

#include <QIODevice>

#include <iostream>
#include <QSerialPort>

SerialPortController::SerialPortController(QString serialConnection,
                                           std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue,
                                           std::queue<std::tuple<int, QByteArray> > serialReadQueue)
{
    this->serialWriteQueue = serialWriteQueue;
    this->serialReadQueue = serialReadQueue;
    this->address = serialConnection;
    this->moveToThread(this);
}

void SerialPortController::run()
{
    std::cout << "------------SerialPortController------------" << std::endl;
    std::cout << "Er rennt" << std::endl;
    /*
    Continuously monitor the serialWriteQueue for commands from other Python processes to be sent to the Magstim.
    When requested, will return the automated reply from the Magstim unit to the calling process via the serialReadQueue.
    N.B. This should be called via start() from the parent Python process.
    */

    // N.B. most of these settings are actually the default in PySerial, but just being careful.

    QSerialPort porto;
    porto.setPortName("COM1");

    bool ok = porto.open(QIODevice::ReadWrite);
        std::cout << "Der Port ist offen :" << (int) ok << std::endl;

    porto.setBaudRate(QSerialPort::Baud9600);
    porto.setDataBits(QSerialPort::Data8);
    porto.setStopBits(QSerialPort::OneStop);
    porto.setParity(QSerialPort::NoParity);
    porto.setFlowControl(QSerialPort::NoFlowControl);
    // Make sure the RTS pin is set to off
    porto.setRequestToSend(false);
    porto.waitForBytesWritten(300);
    std::cout << "Er rennt noch" << std::endl;

    while (true) {
        // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
        QMutexLocker locker(&mutex);
        locker.relock();



        // If the first part of the message is None this signals the process to close the port and stop
        int readBytes = std::get<2>(this->serialWriteQueue.front());
        QString reply = std::get<1>(this->serialWriteQueue.front());
        std::cout << "Qstring und int gelesen" << std::endl;
        QByteArray bmessage = std::get<0>(this->serialWriteQueue.front());
        float message = bmessage.toFloat();
        this->serialWriteQueue.pop();
        char *c;
            std::cout << "Message :" << message << " isnan " << (int) std::isnan(message) << std::endl;
        // If the first part of the message is None this signals the process to close the port and stop
        if(std::isnan(message)) {
                std::cout << "1" << std::endl;
            break;
        }
        // If the first part of the message is a 1 this signals the process to trigger a quick fire using the RTS pin
        else if((int)message == 1) {
                std::cout << "2" << std::endl;
            porto.setRequestToSend(true);
                    std::cout << "Fehlertest" << std::endl;
        }
        // If the first part of the message is a -1 this signals the process to reset the RTS pin
        else if((int)message == -1 ){
                std::cout << "3" << std::endl;
            porto.setRequestToSend(false);
                    std::cout << "Fehlertest" << std::endl;
        }
        // Otherwise, the message is a command string
        else {
            // There shouldn't be any rubbish in the input buffer, but check and clear it just in case
            // if(porto.readBufferSize()!= 0) {    // FIXME funktioniert das überhaupt so? wird es benötigt?
            porto.clear(QSerialPort::AllDirections);
            try {
                // Try writing to the port
                porto.write(bmessage);
                // Read response (this gets a little confusing, as I don't want to rely on timeout to know if there's an error)
                try {
                    porto.read(c,1);
                    bmessage = (c);
                    if (bmessage.at(0) == 'N') {
                        while((int) bmessage.back() > 0) {
                            porto.read(c,1);
                            bmessage.append(c);
                        }
                        // After the end of the version number, read one more byte to grab the CRC
                        porto.read(c,1);
                        bmessage.append(c);
                        // If the first byte is not '?', then the message was understood
                        // so carry on reading in the response (if it was a '?', then this will be the only returned byte).
                    } else if (bmessage.at(0) != '?') {
                        // Read the second byte
                        porto.read(c,1);
                        bmessage.append(c);
                        // If the second returned byte is a '?' or 'S', then the data value supplied either wasn't acceptable ('?') or the command conflicted with the current settings ('S'),
                        // In these cases, just grab the CRC - otherwise, everything is ok so carry on reading the rest of the message
                        if (bmessage.at(1) != '?' && bmessage.at(1) != 'S') {
                            std::cout << "ReadBytes: " << readBytes << std::endl;
                            porto.read(c,1); // FW: FIXME readBytes-2
                            bmessage.append(c);
                        } else {
                            porto.read(c,1);
                            bmessage.append(c);
                        }
                        if (reply.toInt()) {
                            // this->serialReadQueue.push(std::make_tuple(0, bmessage)); //FW: TODO need this?
                            emit updateSerialReadQueue(std::make_tuple(0, bmessage));
                        }
                    }
                } catch (...) { // FW: FIXME
                    // this->serialReadQueue.push(std::make_tuple(SerialPortController::SERIAL_READ_ERROR, bmessage)); //FW: TODO need this?
                    std::cout << "ReadError" << std::endl;
                    emit updateSerialReadQueue(std::make_tuple(SerialPortController::SERIAL_READ_ERROR, bmessage));
                }
            } catch (...) { //FW: FIXME
                // this->serialReadQueue.push(std::make_tuple(SerialPortController::SERIAL_WRITE_ERROR, bmessage)); //FW: TODO need this?
                std::cout << "WriteEror" << std::endl;
                emit updateSerialReadQueue(std::make_tuple(SerialPortController::SERIAL_WRITE_ERROR, bmessage));

            }
        }
        locker.unlock();
    }
    // If we get here, it's time to shutdown the serial port controller
    std::cout << "Close Port" << std::endl;
    porto.close();
    return;
}

void SerialPortController::updateSerialWriteQueue(std::tuple<QByteArray, QString, int> info)
{
    // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
    QMutexLocker locker(&mutex);
    locker.relock();
    std::cout << " upgedaterer SerialWriteQueue" << std::endl;
    this->serialWriteQueue.push(info);
    locker.unlock();
}
