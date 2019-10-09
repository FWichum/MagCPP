#include "serialportcontroller.h"
#include <qiodevice.h>

#include <QIODevice>

#include <iostream>
#include <QSerialPort>

SerialPortController::SerialPortController(QString serialConnection,
                                           std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue,
                                           std::queue<std::tuple<int, QByteArray> > serialReadQueue)
{
    this->m_serialWriteQueue = serialWriteQueue;
    this->m_serialReadQueue = serialReadQueue;
    this->m_address = serialConnection;
    //        this->moveToThread(this);
}


//*************************************************************************************************************

void SerialPortController::run()
{
    // N.B. most of these settings are actually the default in PySerial, but just being careful.
    QSerialPort porto;
    porto.setPortName("COM20"); // this->m_adress FIXME

    bool ok = porto.open(QIODevice::ReadWrite);
    porto.setBaudRate(QSerialPort::Baud9600);
    porto.setDataBits(QSerialPort::Data8);
    porto.setStopBits(QSerialPort::OneStop);
    porto.setParity(QSerialPort::NoParity);
    porto.setFlowControl(QSerialPort::NoFlowControl);
    // Make sure the RTS pin is set to off
    porto.setRequestToSend(false);

    while (true) {
        // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
        QMutexLocker locker(&m_mutex);
        int readBytes;
        QString reply;
        QByteArray bmessage;
        float message;

        // Check if Queue is empty
        if (!m_serialWriteQueue.empty()) {
            int readBytes = std::get<2>(this->m_serialWriteQueue.front());
            QString reply = std::get<1>(this->m_serialWriteQueue.front());
            QByteArray bmessage = std::get<0>(this->m_serialWriteQueue.front());
            this->m_serialWriteQueue.pop();

            message = bmessage.toFloat();
            char c = 65;

            // If the first part of the message is None this signals the process to close the port and stop
            if(reply.contains("closePort")) {
                break;
            }

            // If the first part of the message is a 1 this signals the process to trigger a quick fire using the RTS pin
            else if((int)message == 1) {
                porto.setRequestToSend(true);
            }

            // If the first part of the message is a -1 this signals the process to reset the RTS pin
            else if((int)message == -1 ){
                porto.setRequestToSend(false);
            }

            // Otherwise, the message is a command string
            else {
                // There shouldn't be any rubbish in the input buffer, but check and clear it just in case
                if(porto.readBufferSize()!= 0) {    // FIXME funktioniert das überhaupt so? wird es benötigt?
                    porto.clear(QSerialPort::AllDirections);
                    // porto.flush();
                }
                try {
                    // Try writing to the port
                    QString s_data = QString::fromLocal8Bit(bmessage.data());

                    int i = porto.write(bmessage);
                    bool ok = porto.waitForBytesWritten(300);

                    // Read response (this gets a little confusing, as I don't want to rely on timeout to know if there's an error)
                    try {
                        porto.waitForReadyRead(300);
                        int i = porto.read(&c,1);
                        bmessage = (&c);

                        // Read version number
                        if (bmessage.at(0) == 'N') {
                            while(bmessage.back() != '0') {
                                int i = porto.read(&c,1);
                                bmessage.append(c);

                            }
                            // After the end of the version number, read one more byte to grab the CRC
                            int i = porto.read(&c,1);
                            bmessage.append(c);

                            // If the first byte is not '?', then the message was understood
                            // so carry on reading in the response (if it was a '?', then this will be the only returned byte).
                        } else if (bmessage.at(0) != '?') {
                            // Read the second byte
                            int i = porto.read(&c,1);
                            bmessage.append(c);

                            // If the second returned byte is a '?' or 'S', then the data value supplied either wasn't acceptable ('?') or the command conflicted with the current settings ('S'),
                            // In these cases, just grab the CRC - otherwise, everything is ok so carry on reading the rest of the message
                            if (bmessage.at(1) != '?' && bmessage.at(1) != 'S') {
                                int i = porto.read(&c,readBytes-2); // FW: FIXME readBytes-2
                                bmessage.append(c);

                            } else {
                                int i = porto.read(&c,1);
                                bmessage.append(c);
                            }
                        }

                        if (!reply.isEmpty()) {
                            QString s_data = QString::fromLocal8Bit(bmessage.data());
//                            std::cout << "Gesamt ... " << s_data.toStdString() << std::endl;
                            std::string useless(s_data.toStdString()); // FW: Why is this needed? FIXME without these 2 lines code will not work :O
                            emit updateSerialReadQueue(std::make_tuple(0, bmessage));
                        }

                    } catch (...) { // FW: FIXME
                        emit updateSerialReadQueue(std::make_tuple(SerialPortController::SERIAL_READ_ERROR, bmessage));
                    }
                } catch (...) { //FW: FIXME
                    emit updateSerialReadQueue(std::make_tuple(SerialPortController::SERIAL_WRITE_ERROR, bmessage));

                }
            }
        }
    }

    // If we get here, it's time to shutdown the serial port controller
    porto.close();
}


//*************************************************************************************************************

void SerialPortController::updateSerialWriteQueue(sendInfo info)
{
    // This locker will lock the mutex until it is destroyed, i.e. when this function call goes out of scope
    QMutexLocker locker(&m_mutex);
    this->m_serialWriteQueue.push(info);
}
