#include <iostream>

#include "magstim.h"
#include "rapid.h"

int main()
{
    std::cout << std::endl << "Test." << std::endl;

    Rapid myMag("/dev/ttyS1",0,"",240,std::make_tuple(7,2,0)); // /dev/ttyS0
    int error=0;
    myMag.connect(error);
    std::cout << "Error :" << error;

    return 0;
}
