#include <iostream>

#include "magstim.h"
#include "rapid.h"
#include <ctime>

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
    Rapid myMag("COM20",0,"",240,std::make_tuple(7,2,0)); // COM1
    int error = 0;

    myMag.connect(error);

    std::map<QString, std::map<QString, double> > xyz;
    xyz = myMag.getParameters(error);
    std::cout << "Power : " << xyz["rapidParam"]["power"] << std::endl;
    std::cout << "Frequency : " << xyz["rapidParam"]["frequency"] << std::endl;
    std::cout << "nPulses : " << xyz["rapidParam"]["nPulses"] << std::endl;
    std::cout << "Duration : " << xyz["rapidParam"]["duration"] << std::endl;
    std::cout << "Wait : " << xyz["rapidParam"]["wait"] << std::endl;


    myMag.setPower(20, false, xyz, error);

    myMag.arm(false,xyz, error);

    std::cout << "Armed, underControll, ReadyToFire : " << myMag.isArmed() << myMag.isUnderControl() << myMag.isReadyToFire() << std::endl;

    myMag.ignoreCoilSafetySwitch(error);

    QThread::sleep(1);
	
    myMag.quickFire(error);
    myMag.resetQuickFire();
	QThread::msleep(20);
	myMag.quickFire(error);
    myMag.resetQuickFire();

    QThread::sleep(2);
	
    myMag.disconnect(error);

    std::cout << "---------------------------" << std::endl;
    std::cout << "Termination error :" << error << std::endl;

    return a.exec();
}
