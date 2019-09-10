#ifndef CONNECTIONROBOT_H
#define CONNECTIONROBOT_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "serialportcontroller.h"

#include <queue>
#include <ctime>
#include <tuple>

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QByteArray>
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>

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

class ConnectionRobot : public QThread
{
     Q_OBJECT

public:
    //=========================================================================================================
    /**
    * Constructs a ConnectionRobot
    *
    * @param[in] serialWriteQueue           TODO Doxygen
    * @param[in] updateRobotQueue           TODO Doxygen
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
    * Check the time
    *
    * @return the time
    */
    clock_t defaultTimer();

    //=========================================================================================================
    /**
    * Set the command which will be send to the MagStim unit
    *
    * @param[in] connectionCommand          TODO Doxygen
    */
    void setCommand(std::tuple<QByteArray, QString, int> connectionCommand);

private:
    std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue;
    std::queue<float> updateRobotQueue;
    bool stopped;
    bool paused;
    double nextPokeTime;
    std::tuple<QByteArray, QString, int> connectionCommand;
    QMutex mutex;


public slots:
    //=========================================================================================================
    /**
    * Updates the update queue to control the robot
    *
    * @param[in] info                       TODO Doxygen
    */
    void updateUpdateRobotQueue(const float info);

signals:
    //=========================================================================================================
    /**
    * Send a message to the Magstim unit.
    */
    void updateSerialWriteQueue(const sendInfo info);
};

#endif // CONNECTIONROBOT_H
