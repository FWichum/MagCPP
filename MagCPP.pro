#//=============================================================================================================
#/**
#* @file     connectionrobot.h
#* @author   Hannes Oppermann <hannes.oppermann@tu-ilmenau.de>;
#*           Felix Wichum <felix.wichum@tu-ilmenau.de>
#* @version  1.0
#* @date     November, 2019
#*
#* @section  LICENSE
#*
#* This software was derived from the python toolbox MagPy by N. McNair
#* Copyright (C) 2019, Hannes Oppermann and Felix Wichum. All rights reserved.
#*
#* GNU General Public License v3.0 (LICENSE)
#* This program is free software: you can redistribute it and/or modify
#* it under the terms of the GNU General Public License as published by
#* the Free Software Foundation, either version 3 of the License, or
#* (at your option) any later version.
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#* You should have received a copy of the GNU General Public License
#* along with this program.  If not, see <https://www.gnu.org/licenses/>.
#*
#* @brief    This project file builds the MagCPP library.
#*
#*/

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib

TARGET = MagCPP
CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}


DEFINES += MAGCPP_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        devices/magstim.cpp \
        devices/bistim.cpp \
        devices/rapid.cpp \
        connection/connectionrobot.cpp \
        connection/serialportcontroller.cpp

HEADERS += \
        magcpp_global.h \
        devices/magstim.h \
        devices/bistim.h \
        devices/rapid.h \
        connection/connectionrobot.h \
        connection/serialportcontroller.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    configuration/myres.qrc

# Install headers to include directory
header_files.files = $${HEADERS}
header_files.path = $${MNE_INSTALL_INCLUDE_DIR}/magcpp

INSTALLS += header_files
