#ifndef MAGSTIM_H
#define MAGSTIM_H


class MagStim
{
public:
    MagStim();
    void parseMagstimResponse();
    void connect();
    void disconnect();
    void remoteControl();
    void getParameters();
    void setPower();
    void getTemperature();
    void poke();
    void arm();
    void disarm();
    void isArmed();
    void isUnderControl();
    void isReadyToFire();
    void fire();
    void resetQuickFire();
    void quickFire();


private:
    void setupSerialPort();
    void processCommand();
};

#endif // MAGSTIM_H
