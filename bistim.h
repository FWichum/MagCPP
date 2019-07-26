#ifndef BISTIM_H
#define BISTIM_H
#include "magstim.h"

class BiStim : public MagStim
{
public:
    BiStim(QString serialConnection);
};

#endif // BISTIM_H
