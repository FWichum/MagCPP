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

void Rapid::setupSerialPort(QString serialConnection)
{
    if (serialConnection.toLower() == "virtual") {

    }
    else {
        // FIXME: wait for serialPortController
    }
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
    int helper;
    er = this->processCommand("ND", "version", helper, vers);
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

    return this->processCommand("I@", "error", 6, vers);

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
    std::ignore = MagStim::connect();// FIXME: port number for MagStim::connect(port)
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
            getParameters(mes, updateError);
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
        return this->processCommand("b@", "", 3, mes);
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
                QString empty;
                error = this->processCommand("Q@", empty, 3, message);
            }
        }
        else {
            if(receipt) {
            error = this->processCommand("R@", "instr", 3, message);
            }
            else {
                QString empty;
                error = this->processCommand("R@", empty, 3, message);
            }
        }
    }
    else {
        if(enable){
            QString a = this->unlockCode;
            a = a.toLatin1();
            QString b = "Q";
            QString string = b+a;
            if(receipt) {
                error = this->processCommand(string, "instr", 3, message);
            }
            else {
                error = this->processCommand(string, "", 3, message);
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

void Rapid::enhancedPowerMode(bool enable, std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error = MagStim::er, bool receipt)
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

int Rapid::setFreqeuncy(float newFrequency, std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error = MagStim::er, bool receipt)
{
    this->sequenceValidated = false;
    // Convert to tenths of a Hz
    newFrequency *= 10;
    int helpFreq = int(newFrequency);
    // Make sure we have a valid freqeuncy value    
    if (helpFreq - newFrequency != 0) {         // FIXME: comparing float and int
        return MagStim::PARAMETER_PRECISION_ERR;
    }
    std::map<QString, std::map<QString, int> > currentParameters;
    int updateError = 0;
    getParameters(currentParameters, updateError);
    if (updateError) {
        return MagStim::PARAMETER_ACQUISTION_ERR;
    }
    else {
        int maxFrequency = this->MAX_FREQUENCY[this->voltage][this->super][currentParameters["rapidParam"]["power"]];   // FIXME: in python there are not all arguments in MAX_FREQUENCY
        if (newFrequency < 0 || newFrequency > maxFrequency){
            return MagStim::PARAMETER_RANGE_ERR;
        }
    }
    // Send command
    QString string = QString::number(newFrequency).rightJustified(4,'0');
    error = this->processCommand("B"+string, "instr", 4, message);
    // If we didn't get an error, update the other parameters accordingly
    if (error == 0){
        std::map<QString, std::map<QString, int> > currentParameters;
        int updateError = 0;
        getParameters(currentParameters, updateError);
        if(updateError == 0) {
            int duration = currentParameters["rapidParam"]["duration"];
            int frequency = currentParameters["rapidParam"]["frequency"];
            QString string1 = QString::number(duration*frequency).rightJustified(5, '0');
            QString string2 = QString::number(duration*frequency).rightJustified(4, '0');
            QString string3 = "D";
            if(std::get<0>(this->version) >= 9){
                QString string = string3 + string1;
                updateError = this->processCommand(string, "instrRapid", 4, currentParameters);
            }
            else {
                QString string = string3 + string2;
                updateError = this->processCommand(string, "instrRapid", 4, currentParameters);
            }
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

int Rapid::setNPulses(int newPulses, std::map<QString, std::map<QString, int> > &message = MagStim::mes, bool receipt)
/*
Set number of pulses in rTMS pulse train.

        N.B. Changing the NPulses parameter will automatically update the Duration parameter (this cannot exceed 10 s) based on the current Frequency parameter setting.

        Args:
        newNPulses (int): new number of pulses (Version 9+: 1-6000; Version 7+: ?; Version 5+: 1-1000?)
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
    this->sequenceValidated = false;

    // Mage sure we have a valid number of pulses value
    if(newPulses % 1) {
        return MagStim::PARAMETER_FLOAT_ERR;
    }
    else if (0 > newPulses || newPulses > 6000) {
        return MagStim::PARAMETER_RANGE_ERR;
    }

    // Send command
    int error;
    QString string1 = QString::number(newPulses).rightJustified(5, '0');
    QString string2 = QString::number(newPulses).rightJustified(4, '0');
    QString string3 = "D";
    if(std::get<0>(this->version) >= 9) {
        QString string = string3 + string1;
        error = this->processCommand(string, "instr", 4, message);
    }
    else {
        QString string = string3 + string2;
        error = this->processCommand(string, "instr", 4, message);
    }

    // If we didn't get an error, update the other parameters accordingly
    if(error == 0) {
        std::map<QString, std::map<QString, int> > currentParameters;
        int updateError = 0;
        getParameters(currentParameters, updateError);
        int nPulses = currentParameters["rapidParam"]["nPulses"];
        int frequency = currentParameters["rapidParam"]["frequency"];
        QString string1 = QString::number(nPulses/frequency).rightJustified(4, '0');
        QString string2 = QString::number(nPulses/frequency).rightJustified(3, '0');
        QString string3 = "[";
        if(updateError == 0) {
            if(std::get<0>(this->version) >= 9) {
                QString string = string3 + string1;
                if(receipt){
                    updateError = this->processCommand(string, "instrRapid", 4, currentParameters);
                }
                else {
                    updateError = this->processCommand(string, "", 4, currentParameters);
                }
            }
            else {
                QString string = string3 + string2;
                if(receipt){
                    updateError = this->processCommand(string, "instrRapid", 4, currentParameters);
                }
                else {
                    updateError = this->processCommand(string, "", 4, currentParameters);
                }
            }
            if(updateError){
                return MagStim::PARAMETER_UPDATE_ERR;
            }
        }
        else {
            return MagStim::PARAMETER_ACQUISTION_ERR;
        }
    }
    if(receipt){
        return error;
    }
}

int Rapid::setDuration(float newDuration, std::map<QString, std::map<QString, int> > &message = MagStim::mes, bool receipt)
/*
Set duration of rTMS pulse train.

        N.B. Changing the Duration parameter will automatically update the NPulses parameter based on the current Frequency parameter setting.

        Args:
        newDuration (int/float): new duration of pulse train in seconds (Version 9+: 1-600; Version 7+: ?; Version 5+: 1-10?); decimal values are allowed for durations up to 30s
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
    this->sequenceValidated = false;

    // Convert to tenths of a second
    newDuration *= 10;
    int helpDuration = int(newDuration);
    // Make sure we have a valid duration value
    if (helpDuration % 1) {
        return MagStim::PARAMETER_PRECISION_ERR;
    }
    if(std::get<0>(this->version) >= 9) {
        if (0 > newDuration || newDuration > 9999) {
            return MagStim::PARAMETER_RANGE_ERR;
        }
    }
    else if (0 > newDuration || newDuration > 999) {
        return MagStim::PARAMETER_RANGE_ERR;
    }

    int error;
    QString string1 = QString::number(newDuration).rightJustified(4, '0');
    QString string2 = QString::number(newDuration).rightJustified(3, '0');
    QString string3 = "[";
    if(std::get<0>(this->version) >= 9) {
        QString string = string3 + string1;
        error = this->processCommand(string, "instrRapid", 4, message);
    }
    else {
        QString string = string3 + string2;
        error = this->processCommand(string, "instrRapid", 4, message);
    }
    if(error == 0) {
        std::map<QString, std::map<QString, int> > currentParameters;
        int updateError = 0;
        getParameters(currentParameters, updateError);
        int duration = currentParameters["rapidParam"]["duration"];
        int frequency = currentParameters["rapidParam"]["frequency"];
        QString string1 = QString::number(duration*frequency).rightJustified(5, '0');
        QString string2 = QString::number(duration*frequency).rightJustified(4, '0');
        QString string3 = "D";
        if(updateError == 0) {
            if(std::get<0>(this->version) >= 9) {
                QString string = string3 + string1;
                updateError = this->processCommand(string, "instrRapid", 4, currentParameters);
            }
            else {
                QString string = string3 + string2;
                updateError = this->processCommand(string, "instrRapid", 4, currentParameters);
            }
            if(updateError){
                return MagStim::PARAMETER_UPDATE_ERR;
            }
        }
        else {
            return MagStim::PARAMETER_ACQUISTION_ERR;
        }
    }
    if(receipt){
        return error;
    }
}

void Rapid::getParameters(std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error  = MagStim::er)
/*
Request current parameter settings from the Rapid.

        Returns:
        :tuple:(error,message):
            error (int): error code (0 = no error; 1+ = error)
            message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'], rMTS setting ['rapid'],
            and parameter setting ['rapidParam'] dicts, otherwise returns an error string
*/
{
    int helpNumber = this->parameterReturnBytes;
    error = this->processCommand("\\@", "rapidParam", helpNumber, message);
}

int Rapid::setPower(int newPower, std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error  = MagStim::er, bool receipt, bool delay)
/*
Set power level for the Rapid.

        N.B. Allow 100 ms per unit drop in power, or 10 ms per unit increase in power.

             Changing the power level can result in automatic updating of the Frequency parameter (if in rTMS mode)

        Args:
        newPower (int): new power level (0-100; or 0-110 if enhanced-power mode is enabled)
        receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)
        delay (bool): enforce delay to allow Rapid time to change Power (defaults to False)

        Returns:
        If receipt argument is True:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing a Rapid instrument status ['instr'] dict, otherwise returns an error string
        If receipt argument is False:
            None
*/
{
    this->sequenceValidated = false;

    // Make sure we have a valid power value
    if(newPower % 1) {
        return MagStim::PARAMETER_FLOAT_ERR;
    }
    else {
        if(this->isEnhanced()) {
            if(0 > newPower || newPower > 110) {
                return MagStim::PARAMETER_RANGE_ERR;
            }
            else if (0 > newPower || newPower > 100) {
                return MagStim::PARAMETER_RANGE_ERR;
            }
        }
    }

    MagStim::setPower(newPower, delay = false, error, "@");             // TODO: Message is missing
    if(error == 0) {
        std::map<QString, std::map<QString, int> > currentParameters;
        int updateError = 0;
        getParameters(currentParameters, updateError);
        if(updateError == 0){
            if(currentParameters["rapid"]["singlePulseMode"] == false) {
                int maxFrequency = this->MAX_FREQUENCY[this->voltage][this->super][currentParameters["rapidParam"]["power"]];
                if(currentParameters["rapidParam"]["frequency"] > maxFrequency) {
                    if(setFreqeuncy(maxFrequency) != 0){
                        return MagStim::PARAMETER_UPDATE_ERR;
                    }
                }
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

int Rapid::setChargeDelay(int newDelay, std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error  = MagStim::er, bool receipt)
/*
Set charge delay duration for the Rapid.

        Args:
        newDelay (int): new delay duration in seconds (Version 10+: 1-10000; Version 9: 1-2000)
        receipt (bool): whether to return occurrence of an error and the automated response from the Rapid unit (defaults to False)

        Returns:
        If receipt argument is True:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing a Rapid instrument status ['instr'] dict, otherwise returns an error string
        If receipt argument is False:
            None
*/
{
    if(std::get<0>(this->version) == 0){
        return MagStim::GET_SYSTEM_STATUS_ERR;
    }
    else if(std::get<0>(this->version) < 9) {
        return MagStim::SYSTEM_STATUS_VERSION_ERR;
    }
    this->sequenceValidated = false;

    // Make sure we have a valid delay duration value
    if(newDelay % 1){
        return MagStim::PARAMETER_FLOAT_ERR;
    }
    QString string1 = QString::number(newDelay).rightJustified(5, '0');
    QString string2 = QString::number(newDelay).rightJustified(4, '0');
    QString string3 = "n";
    if(std::get<0>(this->version) >= 10){
        QString string = string3 + string1;
        error = this->processCommand(string, "systemRapid", 6, message);
    }
    else {
        QString string = string3 + string2;
        error = this->processCommand(string, "instrRapid", 4, message);
    }
    if(receipt) {
        return error;
    }
}

int Rapid::getChargeDelay(std::map<QString, std::map<QString, int> > &message = MagStim::mes, int &error  = MagStim::er)
/*
Get current charge delay duration for the Rapid.

        Returns:
            :tuple:(error,message):
                error (int): error code (0 = no error; 1+ = error)
                message (dict,str): if error is 0 (False) returns a dict containing a Rapid instrument status ['instr'] dict and
                charge delay duration ['chargeDelay'] value, otherwise returns an error string
*/
{
    if(std::get<0>(this->version) == 0){
        return MagStim::GET_SYSTEM_STATUS_ERR;
    }
    else if(std::get<0>(this->version) < 9) {
        return MagStim::SYSTEM_STATUS_VERSION_ERR;
    }

    if(std::get<0>(this->version) > 9) {
        error = this->processCommand("o@", "instrCharge", 8, message);
    }
    else {
        error = this->processCommand("o@", "instrCharge", 7, message);
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
        MagStim::fire(message, error);
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
    // pre-initialization
    std::map<QString, std::map<QString, int> > parameters;
    int error;
    float duration = parameters["rapidParam"]["duration"] / 10;
    float MaxOnTime = Rapid::getRapidMaxOnTime(parameters["rapidParam"]["duration"], parameters["rapidParam"]["frequency"]);
    float TimeHelp;
    if (duration < 60) {
        TimeHelp = duration;
    }
    else {
        TimeHelp = 60;
    }

    getParameters(parameters, error);
    if (error) {
        return MagStim::PARAMETER_ACQUISTION_ERR;
    }
    else if (TimeHelp > MaxOnTime) {
        return MagStim::MAX_ON_TIME_ERR;
    }
    else {
        this->sequenceValidated = true;
        return 0; //in python with "Sequence valid."
    }
}

int Rapid::getSystemStatur(std::map<QString, std::map<QString, int> > &message)
/*
et system status from the Rapid. Available only on software version of 9 or later.

        Returns:
        :tuple:(error,message):
            error (int): error code (0 = no error; 1+ = error)
            message (dict,str): if error is 0 (False) returns a dict containing Rapid instrument status ['instr'],
            rMTS setting ['rapid'], and extended instrument status ['extInstr'] dicts, otherwise returns an error string
*/
{
    if (std::get<0>(this->version) == 0) {
        return MagStim::GET_SYSTEM_STATUS_ERR;
    }
    else if (std::get<0>(this->version) >= 9) {
        int error;
        error = this->processCommand("x@", "systemRapid", 6, message);
        return error;
    }
    else {
        return MagStim::SYSTEM_STATUS_VERSION_ERR;
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



