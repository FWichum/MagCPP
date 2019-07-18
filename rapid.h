#ifndef RAPID_H
#define RAPID_H

#include "magstim.h"

class Rapid : public MagStim
{
public:
    Rapid(QString serialConnection, bool superRapid, QString unlockCode, int voltage, std::tuple<int, int, int> version);

private:
    int DEFAULT_RAPID_TYPE;
    int DEFAULT_VOLTAGE;
    QString DEFAULT_UNLOCK_CODE;
    bool ENFORCE_ENERGY_SAFETY;
    std::tuple<int, int, int> DEFAULT_VIRTUAL_VERSION;
    // MAX_FREQUENCY
    // JOULES

    bool super;
    QString unlockCode;
    int voltage;
    std::tuple<int, int, int> version;
    float parameterReturnBytes;
    bool sequenceValidated;
    bool repetitiveMode;
};

#endif // RAPID_H
