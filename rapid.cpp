#include "rapid.h"

Rapid::Rapid(QString serialConnection, int superRapid, QString unlockCode, int voltage, std::tuple<int, int, int> version) :
    MagStim(serialConnection)
{
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
    // parse from yaml file
    this->DEFAULT_RAPID_TYPE = 0;
    this->DEFAULT_VOLTAGE = 240;
    this->DEFAULT_UNLOCK_CODE = "";
    this->ENFORCE_ENERGY_SAFETY = true;
    this->DEFAULT_VIRTUAL_VERSION = std::make_tuple(5,0,0);

    // TODO:
    // MAX_FREQUENCY
    // JOULE

    Rapid(serialConnection,this->DEFAULT_RAPID_TYPE,this->DEFAULT_UNLOCK_CODE,this->DEFAULT_VOLTAGE,this->DEFAULT_VIRTUAL_VERSION);

}
