#include <iostream>

#include "magstim.h"
#include "rapid.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    std::cout << std::endl << "Test." << std::endl;

    Rapid myMag("/dev/ttyS1",0,"",240,std::make_tuple(7,2,0)); // COM1
    int error=0;
    myMag.connect(error);
    std::cout << "---------------------------" << std::endl;
    std::cout << "Abschluss Fehler :" << error << std::endl;

    return a.exec();
}
