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
* @brief    Definition of the Bistim class.
*/

#include "../devices/bistim.h"

BiStim::BiStim(QString serialConnection) :
    MagStim (serialConnection)
{

}
