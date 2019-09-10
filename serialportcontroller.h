#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <queue>

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QByteArray>
#include <QMutexLocker>
#include <QMutex>

//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

typedef std::tuple<QByteArray, QString, int> sendInfo;
typedef std::tuple<int, QByteArray> reciveInfo;
Q_DECLARE_METATYPE(sendInfo);
Q_DECLARE_METATYPE(reciveInfo);

//=============================================================================================================
/**
* The class creates a Thread which has direct control of the serial port. Commands for relaying via the
* serial port are received from signals.
*
* @brief Controls the serial port.
*/

class SerialPortController : public QThread
{
    Q_OBJECT

public:
    //=========================================================================================================
    /**
    * Constructs a SerialPortController
    *
    * @param[in] serialConnection           The serial port
    * @param[in] serialWriteQueue           TODO Doxygen
    * @param[in] serialReadQueue            TODO Doxygen
    */
    SerialPortController(QString serialConnection,
                         std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue,
                         std::queue<std::tuple<int, QByteArray>> serialReadQueue);

    //=========================================================================================================
    /**
    * Continuously monitor the serialWriteQueue for commands from other Threads to be sent to the Magstim.
    * When requested, will return the automated reply from the Magstim unit to the calling process via signals.
    *   N.B. This should be called via start()
    */
    void run() override;

private:
    QSerialPort port;
    std::queue<std::tuple<QByteArray, QString, int>> serialWriteQueue;
    std::queue<std::tuple<int, QByteArray>> serialReadQueue;
    QString address;
    QMutex mutex;

    const int SERIAL_WRITE_ERROR = 1; // SERIAL_WRITE_ERR: Could not send the command.
    const int SERIAL_READ_ERROR  = 2; // SERIAL_READ_ERR:  Could not read the magstim response.

public slots:
    //=========================================================================================================
    /**
    * Updates the write queue to send something to Magstim.
    *
    * @param[in] info                       TODO Doxygen
    */
    void updateSerialWriteQueue(sendInfo info);

signals:
    //=========================================================================================================
    /**
    * A message from the Magstim unit was read.
    */
    void updateSerialReadQueue(const reciveInfo &info);
};

#endif // SERIALPORTCONTROLLER_H
