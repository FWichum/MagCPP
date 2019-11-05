//=============================================================================================================
/**
* @file     bistim.cpp
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
* @brief    Definition of the Bistim class.
*/

#include "bistim.h"

BiStim::BiStim(QString serialConnection) :
    MagStim (serialConnection)
{

}
