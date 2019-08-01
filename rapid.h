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
    std::tuple<int, int, int> getVersion(int &er);
    int getErrorCode();
    void connect(int &er);
    void disconnect();
    int rTMSMode(bool enable, std::map<QString,std::map<QString, int>> &message , bool receipt = false);
    int ignoreCoilSafetySwitch(bool receipt = false);
    int remoteControl(bool enable, std::map<QString, std::map<QString, int>> &message = MagStim::mes, int &error = MagStim::er, bool receipt = false);
    void fire(int &error);
    void quickFire(int &error);

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
