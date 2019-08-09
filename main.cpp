#include <iostream>

#include "magstim.h"
#include "rapid.h"

int main()
{
    std::cout << std::endl << "Test." << std::endl;

    Rapid myMag("COM1",0,"",240,std::make_tuple(7,2,0)); // /dev/ttyS0
    int error=0;
    myMag.connect(error);
    std::cout << "Abschluss Fehler :" << error;

    return 0;
}
