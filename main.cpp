#include <iostream>
//#include "yaml-cpp/yaml.h"
#include "yaml/include/yaml-cpp/yaml.h"

int main()
{
    std::cout << std::endl << "Test." << std::endl;

    YAML::Node config = YAML::LoadFile("D:/git/magcpp/MagCPP/rapid_config.yaml");

    const int volt = config["defaultVoltage"].as<int>();

    std::cout << std::endl << "Volt: " << volt <<std::endl;

    return 0;
}
