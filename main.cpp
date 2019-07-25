#include <iostream>

#include <QRegExp>
#include <QStringList>
#include <QString>
#include <queue>
#include <tuple>
#include <list>
#include <map>
#include <QByteArray>
#include "QString"

int main()
{
    std::cout << std::endl << "Test." << std::endl;
    QByteArray reply = "EB";
    std::string s = reply.toStdString();
    std::list<int> teste(s.begin(), s.end());
    std::cout << "Liste : " << teste.front() << ", " << teste.back() <<std::endl;


    // EB --> 69 66
    QString commandString = "EB";
    QByteArray comString = commandString.toLocal8Bit();
    //TODO check?
    std::cout << "String :" << commandString.toStdString() << std::endl;
    std::cout << "Bytes :" << (int) comString.at(0) << " " <<(int) comString.at(1) ;
    if (comString.at(0) == (char)69)
        std::cout << "Wahr!";
    if (comString.contains("EB"))
        std::cout << " COol";
    return 0;
}
