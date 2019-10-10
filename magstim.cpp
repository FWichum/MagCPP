#include "magstim.h"

#include <functional>
#include <QRegExp>
#include <QStringList>

MagStim::MagStim(QString serialConnection, QObject* parent)
    : QObject(parent)
    // m_robot(this->m_sendQueue, this->m_robotQueue)
{
    qRegisterMetaType<std::tuple<QByteArray, QString, int>>("std::tuple<QByteArray, QString, int>");
    qRegisterMetaType<std::tuple<int, QByteArray>>("std::tuple<int, QByteArray>");

    this->m_robot = new ConnectionRobot(this->m_sendQueue, this->m_robotQueue);
    this->setupSerialPort(serialConnection);

    this->m_connected = false;
    this->m_connectionCommand = std::make_tuple(QString("Q@n").toUtf8(),"", 3);
    // m_connection.daemon = true; //FW: TODO daemon
    // m_robot.daemon = true; //FW: TODO daemon
    // auto queryCommand = std::bind(this->remoteControl, true, true);//FW: TODO queryCommand

    // ConnectionRobot sends Message to SerialPortController for Magstim
    QObject::connect(this->m_robot, &ConnectionRobot::updateSerialWriteQueue,
                     this->m_connection, &SerialPortController::updateSerialWriteQueue, Qt::ConnectionType::DirectConnection);
    // Magstim sends commands to ConnectionRobot
    QObject::connect(this,  &MagStim::updateRobotQueue,
                     this->m_robot, &ConnectionRobot::updateUpdateRobotQueue, Qt::ConnectionType::DirectConnection);
    // Message was read, finish waiting
    QObject::connect(this, &MagStim::readInfo,
                     &this->m_loop, &QEventLoop::quit, Qt::ConnectionType::DirectConnection);
}


//*************************************************************************************************************

std::map<QString, std::map<QString, int> > MagStim::parseMagstimResponse(std::list<int> responseString, QString responseType)
{
    std::map<QString, std::map<QString, int>> magstimResponse;

    if (responseType == "version") {
        return magstimResponse;
    } else {
        // Get ASCII code of first data character
        int temp = responseString.front();
        responseString.pop_front();
        // interpret bits
        std::map<QString, int> instr;
        instr["standby"]        =  temp      & 1;
        instr["armed"]          = (temp >>1) & 1;
        instr["ready"]          = (temp >>2) & 1;
        instr["coilPresent"]    = (temp >>3) & 1;
        instr["replaceCoil"]    = (temp >>4) & 1;
        instr["errorPresent"]   = (temp >>5) & 1;
        instr["errorType"]      = (temp >>6) & 1;
        instr["remoteStatus"]   = (temp >>7) & 1;
        magstimResponse["instr"]= instr;
    }

    // If a Rapid system and response includes rTMS status
    if (responseType == "instrRapid" || responseType == "rapidParam" || responseType=="systemRapid") {
        // Get ASCII code of second data character
        int temp = responseString.front();
        responseString.pop_front();
        // interpret bits
        std::map<QString, int> rapid;
        rapid["enhancedPowerMode"]      =  temp      & 1;
        rapid["train"]                  = (temp >>1) & 1;
        rapid["wait"]                   = (temp >>2) & 1;
        rapid["singlePulseMode"]        = (temp >>3) & 1;
        rapid["hvpsuConnected"]         = (temp >>4) & 1;
        rapid["coilReady"]              = (temp >>5) & 1;
        rapid["thetaPSUDetected"]       = (temp >>6) & 1;
        rapid["modifiedCoilAlgorithm"]  = (temp >>7) & 1;
        magstimResponse["rapid"]= rapid;
    }

    std::string str(responseString.begin(), responseString.end());
    QString rs =  QString::fromUtf8(str.data(), str.size());

    // if requesting parameter settings or coil temperature
    if (responseType == "bistimParam") {
        std::map<QString, int> bistimParam;
        bistimParam["powerA"]   = rs.mid(0,2).toInt();
        bistimParam["powerB"]   = rs.mid(3,5).toInt();
        bistimParam["ppOffset"] = rs.mid(6,8).toInt();
        magstimResponse["bistimParam"]= bistimParam;

    } else if (responseType == "magstimParam") {
        std::map<QString, int> magstimParam;
        magstimParam["power"]   = rs.mid(0,3).toInt();
        magstimResponse["magstimParam"]= magstimParam;

    } else if (responseType == "rapidParam") {
        // This is a bit of a hack to determine which software version we're dealing with
        if (responseString.size() == 20) {
            std::map<QString, int> rapidParam;
            rapidParam["power"]     = rs.mid(0,2).toInt();
            rapidParam["frequency"] = rs.mid(3,6).toInt(); //FW: TODO /10
            rapidParam["nPulses"]   = rs.mid(7,11).toInt();
            rapidParam["duration"]  = rs.mid(12,15).toInt(); //FW: TODO /10
            rapidParam["wait"]      = rs.mid(16,19).toInt(); //FW: TODO /10
            magstimResponse["rapidParam"]= rapidParam;
        } else {
            std::map<QString, int> rapidParam;      // responseString 38 50 57 48 = als chr --> 029
            // Power = 29     WICHTIG = 0:3 heißt 0:2
            rapidParam["power"]     = rs.mid(0,2).toInt();
            rapidParam["frequency"] = rs.mid(3,6).toInt(); //FW: TODO /10
            rapidParam["nPulses"]   = rs.mid(7,10).toInt();
            rapidParam["duration"]  = rs.mid(11,13).toInt(); //FW: TODO /10
            rapidParam["wait"]      = rs.mid(14,rs.length()-1).toInt(); //FW: TODO /10
            magstimResponse["rapidParam"]= rapidParam;
        }

    } else if (responseType == "magstimTemp") {
        std::map<QString, int> magstimTemp;
        magstimTemp["coil1Temp"]   = rs.mid(0,2).toInt(); //FW: TODO /10
        magstimTemp["coil2Temp"]   = rs.mid(2,5).toInt(); //FW: TODO /10
        magstimResponse["magstimTemp"]= magstimTemp;

    } else if (responseType == "systemRapid") {
        int temp = responseString.front();
        std::string str(responseString.begin(), responseString.end());
        QString rs =  QString::fromUtf8(str.data(), str.size());
        responseString.pop_front();
        std::map<QString, int> extInstr;
        extInstr["plus1ModuleDetected"]         = temp & 1;
        extInstr["specialTriggerModeActive"]    = (temp >> 1) & 1;
        extInstr["chargeDelaySet"]              = (temp >> 2) & 1;
        magstimResponse["extInstr"]= extInstr;

    } else if (responseType == "error") {
        std::map<QString, int> currentErrorCode;
        currentErrorCode["currentErrorCode"]  = rs.mid(0,rs.length()-2).toInt();
        magstimResponse["currentErrorCode"]= currentErrorCode;

    } else if (responseType == "instrCharge") {
        std::map<QString, int> chargeDelay;
        chargeDelay["currentErrorCode"]  = rs.toInt();
        magstimResponse["chargeDelay"]= chargeDelay;
    }
    return magstimResponse;
}


//*************************************************************************************************************

std::tuple<int, int, int> MagStim::parseMagstimResponse_version(std::list<int> responseString)
{
    // Convert to char except first 2 elements
    char foo[responseString.size()-2]; //FIXME: responseString.size()-2 // 4
    std::list<int>::iterator it = responseString.begin();
    it++; it++;
    int i=0;
    do {
        foo[i] = *it;
        it++; i++;
    } while (it != responseString.end());

    // Read as number
    QString s = foo;
    QStringList versionList = s.split('.');
    for (int i = 0; i<versionList.length(); i++) {
        bool ok = true;
        int hex = versionList.at(i).toInt(&ok, 10);
        if (!ok)
            versionList[i] = "0";
    }

    // Fill with zeros to get subsub version
    std::tuple<int, int, int> magstimResponse;
    if (versionList.length() == 3)
        magstimResponse = std::make_tuple(versionList.at(0).toInt(), versionList.at(1).toInt(), versionList.at(2).toInt());
    if (versionList.length() == 2)
        magstimResponse = std::make_tuple(versionList.at(0).toInt(), versionList.at(1).toInt(), 0);
    if (versionList.length() == 1)
        magstimResponse = std::make_tuple(versionList.at(0).toInt(), 0, 0);
    return magstimResponse;
}


//*************************************************************************************************************

void MagStim::connect(int &error = MagStim::er)
{
    if (!this->m_connected) {
        this->m_connection->start(QThread::Priority::TimeCriticalPriority);

        std::map<QString, std::map<QString, int>> mes;
        remoteControl(true,mes,error);
        std::cout << "Verbunden? " << error << std::endl;
        if (!error) {
            this->m_connected = true;
            this->m_robot->setCommand(this->m_connectionCommand);
            this->m_robot->start(QThread::Priority::TimeCriticalPriority);
            //            this->m_robot->run();
        } else {
            QByteArray qb;
            QString s = "closePort";
            int i;
            emit updateSendQueue(std::make_tuple(qb,s,i));
            if (this->m_connection->isRunning()) {
                this->m_connection->wait(); //FW: FIXME join()
            }
            //Raise MaigstimError FIXME
        }
    }
}


//*************************************************************************************************************

void MagStim::disconnect(int &error = MagStim::er)
{
    if (this->m_connected) {
        std::map<QString, std::map<QString, int>> message;
        this->disarm(message, error);
        emit updateRobotQueue(NAN);

        if (this->m_robot->isRunning()) {
            this->m_robot->wait(); // FW: FIXME join()
        }
        remoteControl(false,message, error);
        emit updateSendQueue(std::make_tuple("","closePort",NULL));

        if (this->m_connection->isRunning()) {
            this->m_connection->wait(); // FW: FXME join()
        }
    }
}


//*************************************************************************************************************

void MagStim::updateReceiveQueue(reciveInfo info)
{
    this->m_receiveQueue.push(info);
    emit readInfo();
}


//*************************************************************************************************************

void MagStim::remoteControl(bool enable, std::map<QString, std::map<QString, int>> &message = MagStim::mes, int &error = MagStim::er)
{
    QString str = "instr";

    if (enable) {
        error = this->processCommand("Q@", str, 3, message);
    } else {
        error = this->processCommand("R@", str, 3, message);
    }

    return;
}


//*************************************************************************************************************

std::map<QString, std::map<QString, int> >MagStim::getParameters(int &error = MagStim::er)
{
    std::map<QString, std::map<QString, int> > mes;
    error = this->processCommand("J@", "magstimParam", 12, mes);

    return mes;
}


//*************************************************************************************************************

void MagStim::setPower(int newPower, bool delay=false, int &error = MagStim::er, QString commandByte = "@", std::map<QString, std::map<QString, int>> &message = MagStim::mes)
{
    // Make sure we have a valid power value
    if (newPower <= 0 || newPower >= 100) {
        error = MagStim::PARAMETER_RANGE_ERR;
        return;
    }

    // If enforcing power change delay, grab current parameters
    int priorPower = 0;
    if (delay) {
        message = getParameters(error);
        if (error) {
            return;
        }
        //FW: FIXME!!!
        try {
            if (commandByte == "@") {
                priorPower = message["bistimParam"]["PowerA"];
            } else {
                priorPower = message["bistimParam"]["PowerB"];
            }
        } catch (...) {
            try {
                priorPower = message["rapidParam"]["power"];
            } catch (...) {
                try {
                    priorPower = message["magstimParam"]["power"];
                } catch (...) {

                }
            }
        }
    }

    QString commandString = commandByte + QString("%1").arg(newPower, 3, 10, QChar('0'));
    QString receiptType = "";

    if (delay) {
        receiptType = "instr";
    }

    error = this->processCommand(commandString + "n", receiptType, 3, message);

    // If we're meant to delay (and we were able to change the power), then enforce if prior power settings are available
    if (delay && !error) {
        if (!error) {
            if (newPower > priorPower) {
                // FW: TODO sleep
            } else {
                // FW: TODO sleep
            }
        } else {
            error = MagStim::PARAMETER_UPDATE_ERR;
        }
    }
}


//*************************************************************************************************************

std::map<QString, std::map<QString, int> > MagStim::getTemperature(int &error = MagStim::er)
{
    std::map<QString, std::map<QString, int> > mes;
    error = this->processCommand("F@", "magstimTemp", 9, mes);
    return mes;
}


//*************************************************************************************************************

void MagStim::poke()
{

}


//*************************************************************************************************************

void MagStim::arm(bool delay = false, std::map<QString, std::map<QString, int>> &message = MagStim::mes, int &error = MagStim::er)
{
    error = this->processCommand("EB", "instr", 3, message);
    if (delay) {
        // sleep FIXME
    }
    return;
}


//*************************************************************************************************************

void MagStim::disarm(std::map<QString, std::map<QString, int>> &message = MagStim::mes, int &error = MagStim::er)
{
    QString str = "instr";
    error = this->processCommand("EA", str, 3, message);
    return;
}


//*************************************************************************************************************

bool MagStim::isArmed()
{
    int error;
    std::map<QString, std::map<QString, int>> mes;
    remoteControl(true,mes,error);

    if (error) {
        return false;
    }

    if (mes["instr"]["armed"] || mes["instr"]["remoteStatus"]) {
        return true;
    } else {
        return false;
    }
}


//*************************************************************************************************************

bool MagStim::isUnderControl()

{
    int error = 0;
    std::map<QString, std::map<QString, int>> mes;
    remoteControl(true,mes,error);

    if (error) {
        return false;
    }
    std::cout << "UnderControl Zwischen Fehler :" << error << std::endl;
    if (mes["instr"]["remoteStatus"]) {
        return true;
    } else {
        return false;
    }
}


//*************************************************************************************************************

bool MagStim::isReadyToFire()
{
    int error;
    std::map<QString, std::map<QString, int>> mes;
    remoteControl(true,mes,error);

    if (error) {
        return false;
    }

    if (mes["instr"]["ready"]) {
        return true;
    } else {
        return false;
    }
}

void MagStim::fire(std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error = MagStim::er)
{
    QString str = "instr";
    error =  this->processCommand("EHn", str, 3, message);
    return;
}


//*************************************************************************************************************

void MagStim::resetQuickFire()
{
    emit updateSendQueue(std::make_tuple(QByteArray::number(-1), "", 0));
}


//*************************************************************************************************************

void MagStim::quickFire()
{
    emit updateSendQueue(std::make_tuple(QByteArray::number(1), "", 0));
}


//*************************************************************************************************************

void MagStim::setupSerialPort(QString serialConnection)
{
    // FW: TODO in case of virtual load virtual
    this->m_connection = new SerialPortController(serialConnection, this->m_sendQueue, this->m_receiveQueue);

    // Read response from Magstim
    QObject::connect(this->m_connection, &SerialPortController::updateSerialReadQueue,
                     this, &MagStim::updateReceiveQueue);
    // Write message to Magstim
    QObject::connect(this, &MagStim::updateSendQueue,
                     this->m_connection, &SerialPortController::updateSerialWriteQueue);
}


//*************************************************************************************************************

int MagStim::processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version, std::map<QString, std::map<QString, int>> &message)
{
    QByteArray comString = commandString.toLocal8Bit();
    QByteArray reply;

    if (this->m_connected || comString.at(0) == (char)81 || comString.at(0) == (char)82 || comString.at(0) == (char)74 || comString.at(0) == (char)70 || comString.contains("EA") || ( comString.at(0) == (char)92 && this->m_parameterReturnByte != 0 )  ) {
        sendInfo info;
        QByteArray qb = comString.append(calcCRC(comString)); // FW: FIXME most likely // before: comString + calcCRC(comString)

        info = std::make_tuple(qb, receiptType, readBytes);
        emit updateSendQueue(info);

        if (!receiptType.isEmpty()) {
            m_loop.exec();
            int error = std::get<0>(this->m_receiveQueue.front());

            reply = std::get<1>(this->m_receiveQueue.front());
            this->m_receiveQueue.pop(); // FW: FIXME
            std::cout << "Empfangen: " << reply.toStdString() << std::endl;
            if (error) {
                return error; // FW: Change for C++ Reasons to just error
            } else {
                if (reply.at(0) == (char)63) {
                    return MagStim::INVALID_COMMAND_ERR;
                } else if (reply.at(1) == (char)63)  {
                    return MagStim::INVALID_DATA_ERR;
                } else if (reply.at(1) == (char)83) {
                    return MagStim::COMMAND_CONFLICT_ERR;
                } else if (reply.at(0) != comString.at(0)) {
                    return MagStim::INVALID_CONFIRMATION_ERR;
                } else if (calcCRC(reply.mid(0,reply.length()-1)) != reply.at(reply.length()-1)) { //TODO check
                    return MagStim::CRC_MISMATCH_ERR;
                }
            }
        }

        if (this->m_connected) {
            if (comString.at(0) == (char)0x82) {
                emit updateRobotQueue(-1);
            } else if (comString.left(2).contains("EA") ) {
                emit updateRobotQueue(1);
            } else if (comString.left(2).contains("EB") ) {
                emit updateRobotQueue(2);
            }  else {
                emit updateRobotQueue(0);
            }
        }

        std::string s = reply.toStdString();
        std::list<int> intlist(s.begin(), s.end());

        if (receiptType == "version") {
            version = this->parseMagstimResponse_version(intlist);
        } else {
            message = this->parseMagstimResponse(intlist, receiptType);
        }
        return 0;
    } else {
        return MagStim::NO_REMOTE_CONTROL_ERR;
    }
}


//*************************************************************************************************************

int MagStim::processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version)
{
    if (receiptType == "version") {
        std::map<QString, std::map<QString, int>> mes;
        return processCommand(commandString, receiptType, readBytes, version, mes);
    } else {
        return MagStim::INVALID_DATA_ERR;
    }
}


//*************************************************************************************************************

int MagStim::processCommand(QString commandString, QString receiptType, int readBytes, std::map<QString, std::map<QString, int>> &message)
{
    if (receiptType == "version") {
        return MagStim::INVALID_DATA_ERR;
    } else {
        std::tuple<int, int, int> vers;
        return processCommand(commandString, receiptType, readBytes, vers, message);
    }
}


//*************************************************************************************************************

char MagStim::calcCRC(QByteArray command)
{
    // Convert command string to sum of ASCII/byte values
    int commandSum = 0;
    for (int i = 0 ; i< command.length() ; i++) {
        commandSum += command.at(i);
    }
    // Convert command sum to binary, then invert and return 8-bit character value
    return (char) (~commandSum & 0xff);
}
