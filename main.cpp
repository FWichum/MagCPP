#include <iostream>
//#include "yaml-cpp/yaml.h"
#include "yaml/include/yaml-cpp/yaml.h"
#include "QFile"
#include "QDir"

int main()
{
    std::cout << std::endl << "Test." << std::endl;

//    QFile file;
  //  file = QDir::currentPath() + "/rapid_system_info.yaml";
    YAML::Node rapid = YAML::LoadFile("D:/git/magcpp/MagCPP/rapid_system_info.yaml");

    std::cout << std::endl << "File read." << std::endl;

    for(YAML::const_iterator it=rapid["joules"].begin();it!=rapid["joules"].end();++it) {
      std::cout << "Playing at " << it->first.as<int>() << " is " << it->second.as<float>() << "\n";
    }

    std::map<std::string, double> JOULES = rapid["joules"].as<std::map<std::string, double>>();

    std::cout << std::endl << "Joules." << std::endl;

    std::map<int, std::map<int, std::map<int, int>>> MAX_FREQUENCY = rapid["maxFrequency"].as<std::map<int, std::map<int, std::map<int, int>>>>();
    std::cout << std::endl << "Freq." << std::endl;

    return 0;
}
