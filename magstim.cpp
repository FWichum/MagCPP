#include "magstim.h"

#include <functional>

MagStim::MagStim(QString serialConnection):
    robot(this->sendQueue,this->robotQueue)
{
    setupSerialPort(serialConnection);
    // connection.daemon = true; //FW: TODO
    // robot.daemon = true; //FW: TODO
    this->connected = false;
    // connectionCommand = (b'Q@n', None, 3) //FW: TODO
    // auto queryCommand = std::bind(this->remoteControl, true, true);//FW: TODO
}

std::map<QString, std::map<QString, int> > MagStim::parseMagstimResponse(std::list<QByteArray> responseString, QString responseType)
// Interprets response sent from the Magstim unit.
{
    std::map<QString, std::map<QString, int>> magstimResponse;
    if (responseType == "version") {
        // FW: TODO something - Regexpression!?
        std::list<QByteArray>::iterator it = responseString.begin();
        while(it != responseString.end()) {
            // ...
            it++;
        }
    } else {
        // Get ASCII code of first data character
        int temp = responseString.front().toInt(); //FW: TODO richtiger Typ?
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
        int temp = responseString.front().toInt(); //FW: TODO richtiger Typ?
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
    // if requesting parameter settings or coil temperature
    if (responseType == "bistimParam") {
        std::map<QString, int> bistimParam;
        bistimParam["powerA"]   = 1; //FW: TODO
        bistimParam["powerB"]   = 1; //FW: TODO
        bistimParam["ppOffset"] = 1; //FW: TODO
        magstimResponse["bistimParam"]= bistimParam;
    } else if (responseType == "magstimParam") {
        std::map<QString, int> magstimParam;
        magstimParam["power"]   = 1; //FW: TODO
        magstimResponse["magstimParam"]= magstimParam;
    } else if (responseType == "rapidParam") {
        // This is a bit of a hack to determine which software version we're dealing with
        if (responseString.size() == 20) {
            std::map<QString, int> rapidParam;
            rapidParam["power"]   = 1; //FW: TODO
            rapidParam["frequency"]   = 1; //FW: TODO
            rapidParam["nPulses"] = 1; //FW: TODO
            rapidParam["duration"] = 1; //FW: TODO
            rapidParam["wait"] = 1; //FW: TODO
            magstimResponse["rapidParam"]= rapidParam;
        } else {
            std::map<QString, int> rapidParam;
            rapidParam["power"]   = 1; //FW: TODO
            rapidParam["frequency"]   = 1; //FW: TODO
            rapidParam["nPulses"] = 1; //FW: TODO
            rapidParam["duration"] = 1; //FW: TODO
            rapidParam["wait"] = 1; //FW: TODO
            magstimResponse["rapidParam"]= rapidParam;
        }
    } else if (responseType == "magstimTemp") {
        std::map<QString, int> magstimTemp;
        magstimTemp["coil1Temp"]   = 1; //FW: TODO
        magstimTemp["coil2Temp"]   = 1; //FW: TODO
        magstimResponse["magstimTemp"]= magstimTemp;
    } else if (responseType == "systemRapid") {
        int temp = responseString.front().toInt(); //FW: TODO richtiger Typ?
        responseString.pop_front();
        std::map<QString, int> extInstr;
        extInstr["plus1ModuleDetected"]         = temp & 1; //FW: TODO
        extInstr["specialTriggerModeActive"]    = (temp >> 1) & 1; //FW: TODO
        extInstr["chargeDelaySet"]              = (temp >> 2) & 1; //FW: TODO
        magstimResponse["extInstr"]= extInstr;
    } else if (responseType == "error") {
        std::map<QString, int> currentErrorCode;
        currentErrorCode["currentErrorCode"]  = 1; //FW: TODO
        magstimResponse["currentErrorCode"]= currentErrorCode;
    } else if (responseType == "instrCharge") {
        std::map<QString, int> chargeDelay;
        chargeDelay["currentErrorCode"]  = 1; //FW: TODO
        magstimResponse["chargeDelay"]= chargeDelay;
    }
    return magstimResponse;
}

void MagStim::connect()
{

}

void MagStim::disconnect()
{

}

void MagStim::remoteControl()
{

}

void MagStim::getParameters()
{

}

void MagStim::setPower()
{

}

void MagStim::getTemperature()
{

}

void MagStim::poke()
{

}

void MagStim::arm()
{

}

void MagStim::disarm()
{

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

void MagStim::fire()
{

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

void MagStim::processCommand()
{

}
