#ifndef RAPID_H
#define RAPID_H
#include "magstim.h"

class Rapid : public MagStim
{
public:
    Rapid(QString serialConnection);
};

#endif // RAPID_H
