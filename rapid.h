#ifndef RAPID_H
#define RAPID_H
#include "magstim.h"

class Rapid : public MagStim
{
public:
    Rapid(QString serialConnection, int superRapid, QString unlockCode, int voltage, std::tuple<int, int, int> version);
    Rapid(QString serialConnection);
    float getRapidMinWaitTime(int power, int nPulses, float frequency);
    float getRapidMaxOnTime(int power, float frequency);
    float getRapidMaxContinuousOperationsFrequency(int power);
    void setupSerialPort(QString serialConnection);
    std::tuple<int, int, int> getVersion(int &er);
    int getErrorCode();
    void connect(int &er);
    void disconnect();
    int rTMSMode(bool enable, std::map<QString,std::map<QString, int>> &message , bool receipt = false);
    int ignoreCoilSafetySwitch(bool receipt);
    void remoteControl(bool enable, std::map<QString, std::map<QString, int>> &message, int &error, bool receipt = false);
    void enhancedPowerMode(bool enable, std::map<QString, std::map<QString, int>> &message, int &error, bool receipt = false);
    bool isEnhanced();  // HO: TODO: QueryCommand is needed
    int setFrequency(float newFrequency, std::map<QString, std::map<QString, int>> &message, int &error, bool receipt = false);
    int setNPulses(int newPulses, std::map<QString, std::map<QString, int>> &message, bool receipt = false);
    int setDuration(float newDuration, std::map<QString, std::map<QString, int>> &message, bool receipt = false);
    void getParameters(std::map<QString, std::map<QString, int>> &message, int &error);
    void setPower(int newPower, bool delay, std::map<QString, std::map<QString, int> > &message, int &error);
    int setChargeDelay(int newDelay, std::map<QString, std::map<QString, int>> &message, int &error, bool receipt = false);
    int getChargeDelay(std::map<QString, std::map<QString, int>> &message, int &error);
    void fire(int &error);
    void quickFire(int &error);
    int validateSequence();
    int getSystemStatur(std::map<QString, std::map<QString, int>> &message);

private:
    void setDefault();

    int DEFAULT_RAPID_TYPE;
    int DEFAULT_VOLTAGE;
    QString DEFAULT_UNLOCK_CODE;
    bool ENFORCE_ENERGY_SAFETY;
    std::tuple<int, int, int> DEFAULT_VIRTUAL_VERSION;
    std::map<int, float> JOULES;
    std::map<int, std::map<int, std::map<int, int>>> MAX_FREQUENCY;
    std::map<QString,std::map<QString, int>> DEFAULT_MESSAGE;

    int super;
    QString unlockCode;
    int voltage;
    std::tuple<int, int, int> version;
    float parameterReturnBytes;
    bool sequenceValidated;
    bool repetitiveMode;
};

#endif // RAPID_H
