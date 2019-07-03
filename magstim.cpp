#include "magstim.h"

#include <functional>

MagStim::MagStim(QString serialConnection):
    robot(this->sendQueue,this->robotQueue)
{
    setupSerialPort(serialConnection);
    // connection.daemon = true; //FW: TODO
    // robot.daemon = true; //FW: TODO
    this->connected = false;
    // connectionCommand = (b'Q@n', None, 3) //FW: TODO
    // auto queryCommand = std::bind(this->remoteControl, true, true);//FW: TODO
}

void MagStim::parseMagstimResponse()
{

}

void MagStim::connect()
{

}

void MagStim::disconnect()
{

}

void MagStim::remoteControl()
{

}

void MagStim::getParameters()
{

}

void MagStim::setPower()
{

}

void MagStim::getTemperature()
{

}

void MagStim::poke()
{

}

void MagStim::arm()
{

}

void MagStim::disarm()
{

}

void MagStim::isArmed()
{

}

void MagStim::isUnderControl()
{

}

void MagStim::isReadyToFire()
{

}

void MagStim::fire()
{

}

void MagStim::resetQuickFire()
{

}

void MagStim::quickFire()
{

}

void MagStim::setupSerialPort(QString serialConnection)
{

}

void MagStim::processCommand()
{

}
