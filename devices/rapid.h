//=============================================================================================================
/**
* @file     rapid.h
* @author   Hannes Oppermann <hannes.oppermann@tu-ilmenau.de>;
*           Felix Wichum <felix.wichum@tu-ilmenau.de>
* @version  1.0
* @date     November, 2019
*
* This software was derived from the python toolbox MagPy by N. McNair
* @section  LICENSE
*
* Copyright (C) 2019, Hannes Oppermann and Felix Wichum. All rights reserved.
*
* GNU General Public License v3.0 (LICENSE)
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
* @brief    Contains the declaration of the Rapid class.
*
*/

#ifndef RAPID_H
#define RAPID_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../magcpp_global.h"
#include "../devices/magstim.h"

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QTextStream>
#include <QMap>

//=============================================================================================================
/**
* This is a sub-class of the parent Magstim class used for controlling Rapid^2 Magstim units. It allows firing
* in either single-pulse mode or rTMS mode. In single-pulse mode, the maximum firing frequency is 1 Hz (0.5 Hz
* if enhanced-power mode is enabled and power is 100 - 110%). To enable rTMS mode, you must first call rTMSMode(True).
* To disable rTMS mode, call rTMSMode(False).
*   N.B. In rTMS mode the maximum frequency allowed is dependent on the power level. Also, there is a dependent
*   relationship between the Duration, NPulses, and Frequency parameter settings. Therefore it is recommended
*   either to seek confirmation of any change in settings or to evaluate allowable changes beforehand. In addition,
*   after each rTMS train there is an enforced delay (minimum 500 ms) before any subsequent train can be initiated
*   or before any rTMS parameter settings can be altered.
*
* @brief This is a sub-class of the parent Magstim class used for controlling Rapid^2 Magstim units
*/

class MAGCPPSHARED_EXPORT Rapid : public MagStim
{
public:
    //=========================================================================================================
    /**
    * Constructs a Rapid. Overloaded function.
    *
    * @param[in] serialConnection           The serial port
    * @param[in] superRapid                 TODO Doxygen
    * @param[in] unlockCode                 if provided, leave empty if not
    * @param[in] voltage                    240 / 115
    * @param[in] version                    software version
    */
    Rapid(QString serialConnection, int superRapid, QString unlockCode, int voltage, std::tuple<int, int, int> version);

    //=========================================================================================================
    /**
    * Constructs a Rapid using defaults settings. Overloaded function.
    *
    * @param[in] serialConnection           The serial port
    */
    Rapid(QString serialConnection);

    //=========================================================================================================
    /**
    * Calculate minimum wait time between trains for given power, frequency, and number of pulses
    *
    * @param[in] power                      TODO Doxygen
    * @param[in] nPulses                    TODO Doxygen
    * @param[in] frequency                  TODO Doxygen
    *
    * @return TODO Doxygen
    */
    float getRapidMinWaitTime(int power, int nPulses, float frequency);

    //=========================================================================================================
    /**
    * Calculate maximum train duration per minute for given power and frequency. If greater than 60 seconds,
    * will allow for continuous operation for up to 6000 pulses.
    *
    * @param[in] power                      TODO Doxygen
    * @param[in] frequency                  TODO Doxygen
    *
    * @return TODO Doxygen
    */
    float getRapidMaxOnTime(int power, float frequency);

    //=========================================================================================================
    /**
    * Calculate maximum frequency that will allow for continuous operation (up to 6000 pulses).
    *
    * @param[in] power                      TODO Doxygen
    *
    * @return TODO Doxygen
    */
    float getRapidMaxContinuousOperationsFrequency(int power);

    //=========================================================================================================
    /**
    * Opens the serialportcontroller to connect with the MagStim unit
    *
    * @param[in] serialConnection           SerialPort (Example: Windows: "COM1"; Linux: "/dev/ttyS0")
    */
    void setupSerialPort(QString serialConnection);

    //=========================================================================================================
    /**
    * Get Magstim software version number. This is needed when obtaining parameters from the Magstim.
    *
    * @param[in] er                         [opt] Chatches error code
    *
    * @return software version (X, Y, Z)
    */
    std::tuple<int, int, int> getVersion(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Get current error code from Rapid.
    *
    * @return error code
    */
    int getErrorCode();

    //=========================================================================================================
    /**
    * Connect to the Rapid.
    * This starts the serial port controller, as well as a process that constantly keeps in
    * contact with the Rapid so as not to lose control. It also collects the software version number of the
    * Rapid in order to send the correct command for obtaining parameter settings.
    *
    * @param[in] error                      [opt] Chatches error code
    */
    void connect(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Disconnect from the Magstim.
    * This stops maintaining contact with the Magstim and turns the serial port controller off.
    *
    * @param[in] error                      [opt] Chatches error code
    */
    void disconnect(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * This is a helper function to enable/disable rTMS mode.
    *
    * @param[in] enable                     whether to enable (True) or disable (False) control
    * @param[in] message                    Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    */
    void rTMSMode(bool enable, std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    * This allows the stimulator to ignore the state of coil safety interlock switch.
    *
    * @param[in] error                      [opt] Chatches error code
    *
    * @return TODO Doxygen
    */
    int ignoreCoilSafetySwitch(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Enable/Disable remote control of stimulator. Disabling remote control will first disarm the Rapid unit.
    *
    * @param[in] enable                     whether to enable (True) or disable (False) control
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    */
    void remoteControl(bool enable, std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    *  Enable/Disable enhanced power mode; allowing intensity to be set to 110%.
    *   N.B. This can only be enabled in single-pulse mode, and lowers the maximum firing frequency to 0.5 Hz.
    *   Disabling will automatically reduce intensity to 100% if over
    *
    * @param[in] enable                     whether to enable (True) or disable (False) control
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    */
    void enhancedPowerMode(bool enable, std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    *  Helper function that returns True if the Rapid is in enhanced power mode, False if not if it could not be determined.
    *
    * @return true if Rapid is in enhanced power mode
    */
    bool isEnhanced();

    //=========================================================================================================
    /**
    * Set frequency of rTMS pulse train.
    *   N.B. Changing the Frequency will automatically update the NPulses parameter based on the current Duration parameter setting.
    *   The maximum frequency allowed depends on the current Power level and the regional power settings (i.e., 115V vs. 240V)
    *
    * @param[in] newFrequency               new frequency of pulse train in Hertz (0-100 for 240V systems, 0-60 for 115V systems);
    *                                       decimal values are allowed for frequencies up to 30Hz
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    *
    * @return TODO Doxygen
    */
    int setFrequency(float newFrequency, std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Set number of pulses in rTMS pulse train.
    *   N.B. Changing the NPulses parameter will automatically update the Duration parameter (this cannot exceed 10 s)
    *   based on the current Frequency parameter setting.
    *
    * @param[in] newPulses                  new number of pulses (Version 9+: 1-6000; Version 7+: ?; Version 5+: 1-1000?)
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    *
    * @return TODO Doxygen
    */
    int setNPulses(int newPulses, std::map<QString, std::map<QString, double> > &message = MagStim::mes);

    //=========================================================================================================
    /**
    * Set duration of rTMS pulse train.
    *   N.B. Changing the Duration parameter will automatically update the NPulses parameter based on the current
    *   Frequency parameter setting.
    *
    * @param[in] newDuration                new duration of pulse train in seconds (Version 9+: 1-600; Version 7+: ?;
    *                                       Version 5+: 1-10?); decimal values are allowed for durations up to 30s
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    *
    * @return TODO Doxygen
    */
    int setDuration(float newDuration, std::map<QString, std::map<QString, double> > &message = MagStim::mes);

    //=========================================================================================================
    /**
    * Request current parameter settings from the Rapid.
    *
    * @param[in] error                      [opt] Chatches error code
    *
    * @return TODO Doxygen
    */
    std::map<QString, std::map<QString, double> > getParameters(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Set power level for Magstim.
    *   N.B. Allow 100 ms per unit drop in power, or 10 ms per unit increase in power.
    *   Changing the power level can result in automatic updating of the Frequency parameter (if in rTMS mode)
    *
    * @param[in] newPower                   new power level (0-100)
    * @param[in] delay                      [opt] enforce delay to allow Rapid time to change Power (defaults to False)
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    */
    void setPower(int newPower, bool delay, std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Set charge delay duration for the Rapid.
    *
    * @param[in] newDelay                   new delay duration in seconds (Version 10+: 1-10000; Version 9: 1-2000)
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    *
    * @return TODO Doxygen
    */
    int setChargeDelay(int newDelay, std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Get current charge delay duration for the Rapid.
    *
    * @param[in] message                    [opt] Rapid parameters (see more: MagStim::parseMagstimResponse)
    * @param[in] error                      [opt] Chatches error code
    *
    * @return TODO Doxygen
    */
    int getChargeDelay(std::map<QString, std::map<QString, double> > &message = MagStim::mes, int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Fire the stimulator. This overrides the base Magstim method in order to check whether rTMS mode is active,
    * and if so whether the sequence has been validated and the min wait time between trains has elapsed
    *   N.B. Will only succeed if previously armed.
    *
    * @param[in] error                      [opt] Chatches error code
    */
    void fire(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Trigger the stimulator to fire with very low latency using the RTS pin and a custom serial connection.
    */
    void quickFire(int &error = MagStim::er);

    //=========================================================================================================
    /**
    * Validate the energy consumption for the current rTMS parameters for the Rapid.
    * This must be performed before running any new sequence, otherwise calling fire() will return an error.
    *
    * @return TODO Doxygen
    */
    int validateSequence();

    //=========================================================================================================
    /**
    * Get system status from the Rapid. Available only on software version of 9 or later.
    *
    * @param[in] message                    Rapid parameters (see more: MagStim::parseMagstimResponse)
    *
    * @return TODO Doxygen
    */
    int getSystemStatur(std::map<QString, std::map<QString, double> > &message = MagStim::mes);

private:
    //=========================================================================================================
    /**
    * Set member variables as default settings. Read from yaml file if possible.
    */
    void setDefault();

    int DEFAULT_RAPID_TYPE;
    int DEFAULT_VOLTAGE;
    QString DEFAULT_UNLOCK_CODE;
    bool ENFORCE_ENERGY_SAFETY;
    std::tuple<int, int, int> DEFAULT_VIRTUAL_VERSION;
    std::map<QString,std::map<QString, int>> DEFAULT_MESSAGE;
    QMap<QString, QVariant> JOULES;
    QMap<QString, QMap<QString, QMap<QString, QVariant>>> MAX_FREQUENCY;

    int m_super;                              /**< Super Radpid Mode */
    QString m_unlockCode;                     /**< Unlock code */
    int m_voltage;                            /**< Voltage of the unit */
    std::tuple<int, int, int> m_version;      /**< Software version (X, Y, Z) */
    bool m_sequenceValidated;                 /**< Magstim allows current settings */
    bool m_repetitiveMode;                    /**< rTMS mode */
};

#endif // RAPID_H
