#include <iostream>

#include "magstim.h"
#include "rapid.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    std::cout << std::endl << "Test." << std::endl;

    Rapid myMag("COM20",0,"",240,std::make_tuple(7,2,0)); // COM1
    int error=0;
    myMag.connect(error);
    std::map<QString, std::map<QString, double> > xyz;
 //   xyz = myMag.getTemperature(error);
//    xyz = myMag.getParameters(error);
 //   myMag.setPower(25, false, xyz, error);

    myMag.arm(false,xyz, error);


    //xyz = myMag.getParameters(error);
  //  xyz = myMag.getTemperature(error);
 //   bool ContError;
  //  ContError = myMag.isUnderControl();
 //   std::cout << "UnderControl :" << ContError << std::endl;
    // myMag.setPower(25, false, xyz, error);
//    // myMag.getVersion(error);
//    myMag.arm(false, xyz, error);
//   // myMag.setPower(25, false, xyz, error);

    myMag.ignoreCoilSafetySwitch(false);
//    std::cout << "nach ignoreCoilSW" << std::endl;
    QThread::sleep(4);
    myMag.quickFire(error);
  //  QThread::sleep(1);
    myMag.resetQuickFire();
    QThread::sleep(1);
    myMag.quickFire(error);
    QThread::sleep(2);
//    std::cout << "nach fire" << std::endl;
    myMag.disconnect();
    std::cout << "---------------------------" << std::endl;
    std::cout << "Abschluss Fehler :" << error << std::endl;

    return a.exec();
}
