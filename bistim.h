//=============================================================================================================
/**
* @file     bistim.h
* @author   Hannes Oppermann <hannes.oppermann@tu-ilmenau.de>;
*           Felix Wichum <felix.wichum@tu-ilmenau.de>
* @version  1.0
* @date     November, 2019
*
* @section  LICENSE
*
* Copyright (C) 2019, Hannes Oppermann and Felix Wichum. All rights reserved.
*
* GNU General Public License v3.0 (LICENSE)
*
* @brief    Contains the declaration of the Bistim class.
*
*/

#ifndef BISTIM_H
#define BISTIM_H
#include "magstim.h"

class BiStim : public MagStim
{
public:
    BiStim(QString serialConnection);
};

#endif // BISTIM_H
