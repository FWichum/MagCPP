//=============================================================================================================
/**
* @file     connectionrobot.h
* @author   Hannes Oppermann <hannes.oppermann@tu-ilmenau.de>;
*           Felix Wichum <felix.wichum@tu-ilmenau.de>
* @version  1.0
* @date     November, 2019
*
* @section  LICENSE
*
* This software was derived from the python toolbox MagPy by N. McNair
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
* @brief    Contains the declaration of the ConnectionRobot class.
*
*/

#ifndef CONNECTIONROBOT_H
#define CONNECTIONROBOT_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../magcpp_global.h"
#include "../connection/serialportcontroller.h"

#include <queue>
#include <ctime>
#include <tuple>
#include <cmath>

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QByteArray>
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>
#include <QEventLoop>
#include <QTimer>

//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

typedef std::tuple<QByteArray, QString, int> sendInfo;
typedef std::tuple<int, QByteArray> reciveInfo;

//=============================================================================================================
/**
* The class creates a thread which sends an 'enable remote control' command to the Magstim via the serialPortController
* thread every 500ms.
*   N.B. Note that all functions except for run, are run in the callers thread
*
* @brief Keeps remote control.
*/

class MAGCPPSHARED_EXPORT ConnectionRobot : public QThread
{
     Q_OBJECT

public:
    //=========================================================================================================
    /**
    * Constructs a ConnectionRobot
    *
    * @param[in] serialWriteQueue           Queue for writing to Magstim
    * @param[in] updateRobotQueue           Queue for controlling the connectionRobot
    */
    ConnectionRobot(std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue,
                    std::queue<float> updateRobotQueue);

    //=========================================================================================================
    /**
    * Continuously send commands to the serialPortController thread at regular intervals, while also monitoring
    * the updateTimeQueue for commands from the parent thread if this should be delayed, paused, or stopped.
    *   N.B. This should be called via start()
    */
    void run() override;

    //=========================================================================================================
    /**
    * Set the command which will be send to the MagStim unit
    *
    * @param[in] connectionCommand          Command to stay in connection with MagStim
    */
    void setCommand(std::tuple<QByteArray, QString, int> connectionCommand);

private:
    std::queue<std::tuple<QByteArray, QString, int>> m_serialWriteQueue;    /**< Queue for writing to Magstim */
    std::queue<float> m_updateRobotQueue;                                   /**< Queue for controlling the connectionRobot */
    bool m_stopped;                                                         /**< Stop the Robot */
    bool m_paused;                                                          /**< Pause the Robot */
    double m_nextPokeTime;                                                  /**< Next time sending a command to the Magstim */
    std::tuple<QByteArray, QString, int> m_connectionCommand;               /**< Command send to the Magstim */
    QMutex m_mutex;                                                         /**< To protect data in this thread */
    QEventLoop m_loop;                                                      /**< Wait for Signals. Execution stops when data arrives. */
    QTimer m_timer;                                                         /**< Counter */


public slots:
    //=========================================================================================================
    /**
    * Updates the update queue to control the robot
    *
    * @param[in] info                       message for Robot (NAN: stop; -1: pause; 0: go on; 1: slow down; 2: speed up)
    */
    void updateUpdateRobotQueue(const float info);

signals:
    //=========================================================================================================
    /**
    * Send a message to the Magstim unit.
    */
    void updateSerialWriteQueue(const sendInfo info);

    //=========================================================================================================
    /**
    * A message was recived. Allows to leave QEventLoop.
    */
    void readInfo();
};

#endif // CONNECTIONROBOT_H
