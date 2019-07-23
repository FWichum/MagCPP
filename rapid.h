#ifndef RAPID_H
#define RAPID_H
#include "magstim.h"

class Rapid : public MagStim
{
public:
    Rapid(QString serialConnection, int superRapid, QString unlockCode, int voltage, std::tuple<int, int, int> version);
    Rapid(QString serialConnection);
    float getRapidMinWaitTime(int power, int nPulses, double frequency);
    float getRapidMaxOnTime(int power, double frequency);
    float getRapidMaxContinuousOperationsFrequency(int power);

private:
    void setDefault();

    int DEFAULT_RAPID_TYPE;
    int DEFAULT_VOLTAGE;
    QString DEFAULT_UNLOCK_CODE;
    bool ENFORCE_ENERGY_SAFETY;
    std::tuple<int, int, int> DEFAULT_VIRTUAL_VERSION;
    std::map<int, float> JOULES;
    std::map<int, std::map<int, std::map<int, int>>> MAX_FREQUENCY;

    int super;
    QString unlockCode;
    int voltage;
    std::tuple<int, int, int> version;
    float parameterReturnBytes;
    bool sequenceValidated;
    bool repetitiveMode;
};

#endif // RAPID_H
