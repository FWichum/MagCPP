#include <iostream>

#include "magstim.h"
#include "rapid.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    std::cout << std::endl << "Test." << std::endl;

    Rapid myMag("COM1",0,"",240,std::make_tuple(7,2,0)); // COM1
    int error=0;
    myMag.connect(error);
    std::map<QString, std::map<QString, int> > xyz;

    myMag.arm(false,xyz, error);



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
