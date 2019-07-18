#ifndef MAGSTIM_H
#define MAGSTIM_H
#include <stdio.h>
#include <iostream>
#include "rs232.h"
#include <math.h>
#include <iomanip>
#include <sstream>


class MagStim
{
public:
    MagStim(std::string port);
    bool connect(std::string port);
    void disconnect();
    bool get_status();

private:
    int bdrate=19200;
    bool encode_command(uint8_t *destination, uint8_t *data);
    int cp_num=16;
    uint8_t command[13]={};
    char mode[4]={'8','N','1',0};
    uint8_t stat_command[10]={0x40,0x30,0x30,0x4D,0x30,0x30,0x34,0x44,0x0D,0x0A}; // stimmt noch nicht
};

#endif // MAGSTIM_H
