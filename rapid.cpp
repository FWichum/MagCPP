#include "rapid.h"
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

float Rapid::getRapidMinWaitTime(int power, int nPulses, float frequency)
{
    float minWaitTime = 0.5;
    float calcWaitTime = (nPulses * ((frequency * this->JOULES[power]) - float(1050.0)));
    if(minWaitTime < calcWaitTime) {
        return calcWaitTime;
    }
    else {
        return minWaitTime;
    }
}

float Rapid::getRapidMaxOnTime(int power, float frequency)
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

std::tuple<int,int,int> Rapid::getVersion(int &er=MagStim::er)
       /*
       Get Magstim software version number. This is needed when obtaining parameters from the Magstim.
       Returns:
       :tuple:(error,message):
           error (int): error code (0 = no error; 1+ = error)
           message (tuple): if error is 0 (False) returns a tuple containing the version number (in (Major,Minor,Patch) format), otherwise returns an error string
       */
{
    std::tuple<int, int, int> vers;
    er = this->processCommand("ND", "version", 0, vers); // HO: TODO: 0 or NONE?!
   // If we didn't receive an error, update the version number and the number of bytes that will be returned by a getParameters() command
    if (er == 0) {
        this->version = vers;
        if (std::get<0>(this->version) >= 9) {
            this->parameterReturnBytes = 24;
        }
        else if (std::get<0>(this->version) >= 7) {
            this->parameterReturnBytes = 22;
        }
        else {
            this->parameterReturnBytes = 21;
        }
    }
    return vers;
}

int Rapid::getErrorCode()
    /*
    Get current error code from Rapid.

            Returns:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'] and current error code ['errorCode'] dicts, otherwise returns an error string
    */
{
    std::tuple<int, int, int> vers;

    return this->processCommand("I@", "eror", 6, vers);

}

void Rapid::connect(int &er=MagStim::er)
    /*
    Connect to the Rapid.
    This starts the serial port controller, as well as a process that constantly keeps in contact with the Rapid so as not to lose control.
    It also collects the software version number of the Rapid in order to send the correct command for obtaining parameter settings.

    Args:
    receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)

    Returns:
      :tuple:(error,message):
      error (int): error code (0 = no error; 1+ = error)
       message (str): if error is 0 (False) returns a string containing the version number (in (X,X,X) format), otherwise returns an error string
    */

{
    std::ignore = MagStim::connect("COM1");// HO: TODO: right port
    std::ignore = this->getVersion(er);
    if (er) {
        this->disconnect();
        throw new std::string("Could not determine software version of Rapid. Disconnecting.");
    }
}

void Rapid::disconnect()
{
    /*
    Disconnect from the Magstim.
        This stops maintaining contact with the Magstim and turns the serial port controller off.
    */
    //Just some housekeeping before we call the base magstim class method disconnect
    this->sequenceValidated = false;
    this->repetitiveMode = false;
    return MagStim::disconnect();
}

int Rapid::rTMSMode(bool enable, std::map<QString, std::map<QString, int>> &message = MagStim::mes, bool receipt)
{
    /*
    This is a helper function to enable/disable rTMS mode.

    Args:
    enable (bool): whether to enable (True) or disable (False) control
    receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)

        Returns:
        If receipt argument is True:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'] and rMTS setting ['rapid'] dicts, otherwise returns an error string
        If receipt argument is False:
            None This is a helper function to enable/disable rTMS mode.

        Args:
        enable (bool): whether to enable (True) or disable (False) control
        receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)

        Returns:
        If receipt argument is True:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'] and rMTS setting ['rapid'] dicts, otherwise returns an error string
        If receipt argument is False:
            None
    */
    this->sequenceValidated = false;
    // Durations of 1 or 0 are used to toggle repetitive mode on and off
    if (std::get<0>(this->version) >= 9) {
        if (enable) {
            QString commandString = "0010";
        }
        else {
            QString commandString = "0000";
        }
    }
    else {
        if (enable) {
            QString commandString = "010";
        }
        else {
            QString commandString = "000";
        }
    }
    int error = this->processCommand("commadString", "instrRapid", 4, message);
    if (error == 0) {
        if (enable) {
            this->repetitiveMode = true;
            std::map<QString, std::map<QString, int> > mes;
            int updateError = 0;
            mes = getParameters(updateError);
            if (updateError == 0) {
                if (mes["rapidParam"]["frequency"] == 0) {
                    updateError = this->processCommand("B0010", "instrRapid", 4, mes);
                    if (updateError) {
                        return MagStim::PARAMETER_UPDATE_ERR;
                    }
                    else {
                        return MagStim::PARAMETER_ACQUISTION_ERR;
                    }
                }
                else {
                    this->repetitiveMode = false;
                }
            }
        }
    }
    if (receipt) {
        return error;
    }
}


int Rapid::ignoreCoilSafetySwitch(bool receipt)
/*
This allows the stimulator to ignore the state of coil safety interlock switch.

        Args:
        receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)

        Returns:
        If receipt argument is True:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'] dict, otherwise returns an error string
        If receipt argument is False:
            None
*/
{
    if (receipt) {
        return this->processCommand("b@", "instr", 3, mes);
    }
    else {
        return this->processCommand("b@", "", 3, mes); // HO: TODO: is there a better way to code it without if - else?
    }
}

int Rapid::remoteControl(bool enable, std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error = MagStim::er, bool receipt)
/*
    Enable/Disable remote control of stimulator. Disabling remote control will first disarm the Magstim unit.

            Args:
            enable (bool): whether to enable (True) or disable (False) control
            receipt (bool): whether to return occurrence of an error and the automated response from the Magstim unit (defaults to False)

            Returns:
            If receipt argument is True:
                :tuple:(error,message):
                    error (int): error code (0 = no error; 1+ = error)
                    message (dict,str): if error is 0 (False) returns a dict containing a Magstim instrument status ['instr'] dict, otherwise returns an error string
            If receipt argument is False:
                None
*/
{
    this->sequenceValidated = false;
    if (this->unlockCode.isEmpty()) {
        if(enable){
            if(receipt) {
            error = this->processCommand("Q@", "instr", 3, message);
            }
            else {
                error = this->processCommand("Q@", "", 3, message);
            }
        }
        else {
            if(receipt) {
            error = this->processCommand("R@", "instr", 3, message);
            }
            else {
                error = this->processCommand("R@", "", 3, message);
            }
        }
    }
    else {
        if(enable){
            if(receipt) {
            error = this->processCommand("Q"+ ..., "instr", 3, message);    // HO: TODO: change bytearray and put it in the function
            }
            else {
                error = this->processCommand("Q" + ..., "", 3, message);
            }
        }
        else {
            if(receipt) {
                error = this->processCommand("R@", "instr", 3, message);
            }
            else {
                error = this->processCommand("R@", "", 3, message);
            }
        }
    }
}

void Rapid::enhancedPowerMode(bool enable, std::map<QString, std::map<QString, int> > &message, int &error = MagStim::er, bool receipt)
/*
    Enable/Disable enhanced power mode; allowing intensity to be set to 110%.

            N.B. This can only be enabled in single-pulse mode, and lowers the maximum firing frequency to 0.5 Hz.

                 Disabling will automatically reduce intensity to 100% if over

            Args:
            enable (bool): whether to enable (True) or disable (False) enhanced-power mode
            receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)

            Returns:
            If receipt argument is True:
                :tuple:(error,message):
                    error (int): error code (0 = no error; 1+ = error)
                    message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'] and rMTS setting ['rapid'] dicts, otherwise returns an error string
            If receipt argument is False:
                None
*/
{
    if(enable) {
        if(receipt) {
            error = this->processCommand("^@", "instrRapid", 4, message);
        }
        else {
            error = this->processCommand("^@", "", 4, message);
        }
       }
    else {
        if(receipt) {
            error = this->processCommand("_@", "instrRapid", 4, message);
        }
        else {
            error = this->processCommand("_@", "", 4, message);
        }
    }
    return;
}

int Rapid::setFreqeuncy(float newFrequency, std::map<QString, std::map<QString, int> > &message, int &error, bool receipt)
{
    this->sequenceValidated = false;
    // Convert to tenths of a Hz
    newFrequency *= 10;
    int helpFreq = int(newFrequency);
    // Make sure we have a valid freqeuncy value
    if (helpFreq - newFrequency != 0) {
        return MagStim::PARAMETER_PRECISION_ERR;
    }
    std::map<QString, std::map<QString, int> > currentParameters;
    int updateError = 0;
    currentParameters = getParameters(updateError);
    if (updateError) {
        return MagStim::PARAMETER_ACQUISTION_ERR;
    }
    else {
        int maxFrequency;
        // HO: TODO: maxFrequncy from Rapid::MAX_FREQUENCY???
        //int power = currentParameters["rapidParam"]["power"];
        //Rapid::MAX_FREQUENCY[this->voltage][currentParameters["rapidParam"]["power"]];
        if (newFrequency < 0 || newFrequency > maxFrequency){
            return MagStim::PARAMETER_RANGE_ERR;
        }
    }
    // Send command
    error = this->processCommand("B", "instr", 4, message); // HO: TODO: bytearray(str(int(new...))
    // If we didn't get an error, update the other parameters accordingly
    if (error == 0){
        int updateError = 0;
        currentParameters = getParameters(updateError);
        if(updateError == 0) {
            updateError = this->processCommand("D", "instrRapid", 4, currentParameters); // HO: TODO: bytearray(str(int(new...))
            if (updateError) {
                return MagStim::PARAMETER_UPDATE_ERR;
            }
        }
        else {
            return MagStim::PARAMETER_ACQUISTION_ERR;
        }
    }
    if(receipt) {
        return error;
    }
}

void Rapid::fire(int &error = MagStim::er)
/*
Fire the stimulator. This overrides the base Magstim method in order to check whether rTMS mode is active, and if so whether the sequence has been validated and the min wait time between trains has elapsed

        N.B. Will only succeed if previously armed.

        Args:
        receipt (bool): whether to return occurrence of an error and the automated response from the Magstim unit (defaults to False)

        Returns:
        If receipt argument is True:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing a Magstim instrument status ['instr'] dict, otherwise returns an error string
        If receipt argument is False:
            None
*/
{
    if (this->repetitiveMode && Rapid::ENFORCE_ENERGY_SAFETY && !this->sequenceValidated) {
        error = MagStim::SEQUENCE_VALIDATION_ERR;
    }
    else {
        std::map<QString, std::map<QString, int>> message;
        int error;
        return MagStim::fire(message, error); //HO: TODO: not sure if it is okay to return a (void) fnuction in a void fuction
    }
}

void Rapid::quickFire(int &error = MagStim::er)
/*
   Trigger the stimulator to fire with very low latency using the RTS pin and a custom serial connection.
*/
{
    if(this->repetitiveMode && Rapid::ENFORCE_ENERGY_SAFETY && !this->sequenceValidated) {
        error = MagStim::SEQUENCE_VALIDATION_ERR;
    }
    else {
        MagStim::quickFire();
    }
}

int Rapid::validateSequence()
/*
Validate the energy consumption for the current rTMS parameters for the Rapid.
        This must be performed before running any new sequence, otherwise calling fire() will return an error.

        Returns:
        :tuple:(error,message):
            error (int): error code (0 = no error; 1+ = error)
            message (dict,str): if error is 0 (False) returns 'OK', otherwise returns an error string
*/
{
    std::map<QString, std::map<QString, int> > parameters;
    int error;
    parameters = getParameters(error);
    if (error) {
        return MagStim::PARAMETER_ACQUISTION_ERR;
    }
    else if () {

    }
    }
}





void Rapid::setDefault()
// FW: CPP exclusive for getting default settings in constructor
{
    QString file = QDir::currentPath() + "/rapid_config.yaml";

    if (QFile::exists(file)) {
        YAML::Node config = YAML::LoadFile(file.toStdString());
        this->DEFAULT_RAPID_TYPE = config["defaultRapidType"].as<int>();
        this->DEFAULT_VOLTAGE = config["defaultVoltage"].as<int>();
        this->DEFAULT_UNLOCK_CODE = QString::fromStdString(config["unlockCode"].as<std::string>());
        this->ENFORCE_ENERGY_SAFETY = config["enforceEnergySafety"].as<bool>();
        this->DEFAULT_VIRTUAL_VERSION = std::make_tuple(5,0,0); //FIXME: config["virtualVersionNumber"].as<std::tuple<int, int, int>>();
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



