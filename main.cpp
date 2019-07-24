#include <iostream>

int main()
{
    std::cout << std::endl << "Test." << std::endl;

    int commandSum = 25;
    char x = (~commandSum & 0xff);

    std::cout << "Sum: " << commandSum <<  std::endl << "Char :" << x << "/" << (int) x;
    return 0;
}
