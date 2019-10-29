#include <iostream>

#include "magstim.h"
#include "rapid.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);

    Rapid myMag("COM20",0,"",240,std::make_tuple(7,2,0)); // COM1
    int error=0;

    myMag.connect(error);
    std::cout << "Connect Error :" << error  <<std::endl;

    std::map<QString, std::map<QString, double> > xyz;
    xyz = myMag.getParameters(error);
    std::cout << "GetParameters Error :" << error  <<std::endl;

    myMag.setPower(25, false, xyz, error);
    std::cout << "SetPower Error :" << error  <<std::endl;

    myMag.arm(false,xyz, error);
    std::cout << "Arm Error :" << error  <<std::endl;

    std::cout << "Armed, underControll, ReadyToFire : " << myMag.isArmed() << myMag.isUnderControl() << myMag.isReadyToFire() << std::endl;

    myMag.ignoreCoilSafetySwitch(false);

    QThread::sleep(4);
    myMag.quickFire(error);
    myMag.resetQuickFire();

    QThread::sleep(1);
    myMag.quickFire(error);

    QThread::sleep(2);
    myMag.disconnect();

    std::cout << "---------------------------" << std::endl;
    std::cout << "Abschluss Fehler :" << error << std::endl;

    return a.exec();
}
