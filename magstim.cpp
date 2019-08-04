#include "magstim.h"

#include <functional>
#include <QRegExp>
#include <QStringList>

MagStim::MagStim(QString serialConnection) //:
   // robot(this->sendQueue, this->robotQueue)
{
    this->robot = new connectionRobot(this->sendQueue, this->robotQueue);
    // FW: TODO signals
    this->connect(serialConnection.toStdString());
    this->connection = new serialPortController(serialConnection, this->sendQueue, this->receiveQueue);
    // connection.daemon = true; //FW: TODO
    // robot.daemon = true; //FW: TODO
    this->connected = false;
    // connectionCommand = (b'Q@n', None, 3) //FW: TODO
    // auto queryCommand = std::bind(this->remoteControl, true, true);//FW: TODO
}

std::map<QString, std::map<QString, int> > MagStim::parseMagstimResponse(std::list<int> responseString, QString responseType)
// Interprets response sent from the Magstim unit.
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
            rapidParam["frequency"] = rs.mid(3,6).toInt(); //FW: TODO
            rapidParam["nPulses"]   = rs.mid(7,11).toInt();
            rapidParam["duration"]  = rs.mid(12,15).toInt(); //FW: TODO
            rapidParam["wait"]      = rs.mid(16,19).toInt(); //FW: TODO
            magstimResponse["rapidParam"]= rapidParam;
        } else {
            std::map<QString, int> rapidParam;      // responseString 38 50 57 48 = als chr --> 029
            //FW: TODO      // Power = 29     WICHTIG = 0:3 heißt 0:2
            rapidParam["power"]     = rs.mid(0,2).toInt();
            rapidParam["frequency"] = rs.mid(3,6).toInt(); //FW: TODO
            rapidParam["nPulses"]   = rs.mid(7,10).toInt();
            rapidParam["duration"]  = rs.mid(11,13).toInt(); //FW: TODO
            rapidParam["wait"]      = rs.mid(14,rs.length()-1).toInt(); //FW: TODO
            magstimResponse["rapidParam"]= rapidParam;
        }
    } else if (responseType == "magstimTemp") {
        std::map<QString, int> magstimTemp;
        magstimTemp["coil1Temp"]   = rs.mid(0,2).toInt(); //FW: TODO
        magstimTemp["coil2Temp"]   = rs.mid(2,5).toInt(); //FW: TODO
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
        currentErrorCode["currentErrorCode"]  = rs.mid(0,rs.length()-2).toInt(); //FW: TODO
        magstimResponse["currentErrorCode"]= currentErrorCode;
    } else if (responseType == "instrCharge") {
        std::map<QString, int> chargeDelay;
        chargeDelay["currentErrorCode"]  = rs.toInt();
        magstimResponse["chargeDelay"]= chargeDelay;
    }
    return magstimResponse;
}

std::tuple<int, int, int> MagStim::parseMagstimResponse_version(std::list<int> responseString)
//FW: splitted from parseMagstimResponse to handle different return type for responseString == "version"
{
    // FW: Convert to char except first and last element
    char foo[4]; //FIXME: responseString.size()-2
    std::list<int>::iterator it = responseString.begin();
    it++;
    int i=0;
    do {
        foo[i] = *it;
        it++; i++;
    } while (it != responseString.end());
    // FW: read as number
    QString s = foo;
    QStringList versionList = s.split('.');
    for (int i = 0; i<versionList.length(); i++) {
        bool ok = true;
        int hex = versionList.at(i).toInt(&ok, 10);
        if (!ok)
            versionList[i] = "0";
        std::cout << versionList.at(i).toStdString() << " ";
    }
    // FW: create tuple;
    std::tuple<int, int, int> magstimResponse;
    if (versionList.length() == 3)
        magstimResponse = std::make_tuple(versionList.at(0).toInt(), versionList.at(1).toInt(), versionList.at(2).toInt());
    if (versionList.length() == 2)
        magstimResponse = std::make_tuple(versionList.at(0).toInt(), versionList.at(1).toInt(), 0);
    if (versionList.length() == 1)
        magstimResponse = std::make_tuple(versionList.at(0).toInt(), 0, 0);
    return magstimResponse;
}

bool MagStim::connect(std::string port)
{

    //FIXME: get portnumber from portlist
    bool connected=false;
    cp_num=0;
    connected=RS232_OpenComport(cp_num,bdrate,mode);
    //Check if connection was established
    if(connected){
        return true;
    }
    else {
        return false;
    }
}

void MagStim::disconnect()
{
    RS232_CloseComport(cp_num);
}

bool MagStim::encode_command(uint8_t *destination, uint8_t *data)
{
    //Muss noch angepasst werden
    destination[4]=*data;
        destination[5]=*++data;
        destination[6]=*++data;

        std::stringstream test;
        std::string hexstring;
        test<<std::setbase(16);
        test<<std::uppercase;
        test<<(0xff&(destination[0]+destination[1]+
                destination[2]+destination[3]+
                destination[4]+destination[5]+
                destination[6]+destination[7]+
                destination[8]));
        //std::cout<<test.str();
        hexstring+=test.str();
        destination[9]= hexstring[0];
        destination[10]= hexstring[1];
        return true;
}

bool MagStim::get_status()
{
    RS232_SendBuf(cp_num,stat_command,10);
    return true; //FW FIXME
}

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

std::map<QString, std::map<QString, int> >MagStim::getParameters()
{
    std::map<QString, std::map<QString, int> > mes;
    this->processCommand("J@", "magstimParam", 12, mes);
    return mes;
}

void MagStim::setPower()
{

}

std::map<QString, std::map<QString, int> > MagStim::getTemperature()
{
    std::map<QString, std::map<QString, int> > mes;
    this->processCommand("F@", "magstimTemp", 9, mes);
    return mes;
}

void MagStim::poke()
{

}

void MagStim::arm()
{

}

void MagStim::disarm(std::map<QString, std::map<QString, int>> &message = MagStim::mes, int &error = MagStim::er)
{
    QString str = "instr";
    error = this->processCommand("EA", str, 3, message);
    return;
}

void MagStim::isArmed()
{

}

void MagStim::isUnderControl()
{

}

void MagStim::isReadyToFire()
{

}

void MagStim::fire(std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error = MagStim::er)
{
    QString str = "instr";
    error =  this->processCommand("EH", str, 3, message);
    return;
}

void MagStim::resetQuickFire()
{

}

void MagStim::quickFire()
{

}

void MagStim::setupSerialPort(QString serialConnection)
{

}

int MagStim::processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version, std::map<QString, std::map<QString, int>> &message)
{
    // FW: Main Changes for C++
    // TODO: Return Error oder Tuple oder ... Referenz verwenden!?
    // commandString "/@"
    // EB --> 69 66
    QByteArray comString = commandString.toLocal8Bit();
    QByteArray reply;
    if (this->connected || comString.at(0) == (char)81 || comString.at(0) == (char)82 || comString.at(0) == (char)74 || comString.at(0) == (char)70 || comString.contains("EA") || ( comString.at(0) == (char)92 && this->parameterReturnByte != 0 )  ) {
        std::tuple<QByteArray,QString, int> test;
        //this->sendQueue.push(test);    -------------------- <QByteArray, QString, int>
        if (!receiptType.isEmpty()) {
            // error, reply = self.receiveQueue.get()  ------ <int, QByteArray>
            int error = 0;
            reply = "Test";
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
                } else if (calcCRC(reply.mid(0,reply.length()-2)) != reply.at(reply.length()-1)) { //TODO check
                    return MagStim::CRC_MISMATCH_ERR;
                }
            }
        }
        if (this->connected) {
            if (comString.at(0) == (char)0x82) {
                this->robotQueue.push(-1);
            } else if (comString.left(2).contains("EA") ) {
                this->robotQueue.push(1);
            } else if (comString.left(2).contains("EB") ) {
                this->robotQueue.push(2);
            }  else {
                this->robotQueue.push(0);
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

int MagStim::processCommand(QString commandString, QString receiptType, int readBytes, std::tuple<int, int, int> &version)
{
    if (receiptType == "version") {
        std::map<QString, std::map<QString, int>> mes;
        return processCommand(commandString, receiptType, readBytes, version, mes);
    } else {
        return MagStim::INVALID_DATA_ERR;
    }
}

int MagStim::processCommand(QString commandString, QString receiptType, int readBytes, std::map<QString, std::map<QString, int>> &message)
{
    if (receiptType == "version") {
        return MagStim::INVALID_DATA_ERR;
    } else {
        std::tuple<int, int, int> vers;
        return processCommand(commandString, receiptType, readBytes, vers, message);
    }
}

char MagStim::calcCRC(QByteArray command)
//  Return the CRC checksum for the command string.
{
    // Convert command string to sum of ASCII/byte values
    int commandSum = 0;
    for (int i = 1 ; i< command.length() ; i++) {
        commandSum += command.at(i);
    }
    // Convert command sum to binary, then invert and return 8-bit character value
    return (char) (~commandSum & 0xff);
}
