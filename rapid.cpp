#include "rapid.h"

#include "yaml/include/yaml-cpp/yaml.h"
#include <QDir>
#include <QFile>

Rapid::Rapid(QString serialConnection, int superRapid, QString unlockCode, int voltage, std::tuple<int, int, int> version) :
    MagStim(serialConnection)
{
    this->setDefault();
    this->super = superRapid;
    this->unlockCode = unlockCode;
    this->voltage = voltage;
    if (serialConnection.toLower() == "virtual") {
        this->version = version; //FW: Is it right?
    } else {
        this->version = std::make_tuple(0,0,0); // FW: Is it right to set to (0,0,0)?
    }
    // TODO UnlockCode
    this->parameterReturnBytes = NAN;
    this->sequenceValidated = false;
    this->repetitiveMode = false;
}

Rapid::Rapid(QString serialConnection) :
    MagStim(serialConnection)
{
    this->setDefault();
    Rapid(serialConnection,this->DEFAULT_RAPID_TYPE,this->DEFAULT_UNLOCK_CODE,this->DEFAULT_VOLTAGE,this->DEFAULT_VIRTUAL_VERSION);
}

float Rapid::getRapidMinWaitTime(int power, int nPulses, double frequency)
{
    float minWaitTime = 0.5;
    float calcWaitTime = (nPulses * ((frequency * this->JOULES[power]) - 1050.0));
    if(minWaitTime < calcWaitTime) {
        return calcWaitTime;
    }
    else {
        return minWaitTime;
    }
}

float Rapid::getRapidMaxOnTime(int power, double frequency)
{
    float PulseNum = 63000.0;
    float FreqPow = frequency * this->JOULES[power];
    return PulseNum / FreqPow;
}

float Rapid::getRapidMaxContinuousOperationsFrequency(int power)
{
    float a = 1050.0;
    float b = this->JOULES[power];
    return a / b;
}




void Rapid::setDefault()
// FW: CPP exclusive for getting default settings in constructor
{
    QString file = QDir::currentPath() + "/rapid_config.yaml";

    if (QFile::exists(file)) {
        YAML::Node config = YAML::LoadFile(file.toStdString());
        this->DEFAULT_RAPID_TYPE = config["defaultRapidType"].as<int>();
        this->DEFAULT_VOLTAGE = config["defaultVoltage"].as<int>();
        this->DEFAULT_UNLOCK_CODE = config["unlockCode"].as<QString>();
        this->ENFORCE_ENERGY_SAFETY = config["enforceEnergySafety"].as<bool>();
        this->DEFAULT_VIRTUAL_VERSION = config["virtualVersionNumber"].as<std::tuple<int, int, int>>();
    } else {
        this->DEFAULT_RAPID_TYPE = 0;
        this->DEFAULT_VOLTAGE = 240;
        this->DEFAULT_UNLOCK_CODE = "";
        this->ENFORCE_ENERGY_SAFETY = true;
        this->DEFAULT_VIRTUAL_VERSION = std::make_tuple(5,0,0);
    }
    file = QDir::currentPath() + "/rapid_system_info.yaml";
    YAML::Node rapid = YAML::LoadFile(file.toStdString());
    this->MAX_FREQUENCY = rapid["maxFrequency"].as<std::map<int, std::map<int, std::map<int, int>>>>();
    this->JOULES = rapid["joules"].as<std::map<int, float>>();
}
