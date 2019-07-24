#include <iostream>

#include <QRegExp>
#include <QStringList>
#include <QString>
#include <queue>
#include <tuple>
#include <list>
#include <map>
#include <QByteArray>

int main()
{
    std::cout << std::endl << "Test." << std::endl;

    // ---------------------------------------------------
    // Version
    // 86 55 46 50 32 0
    std::list<int> responseString;
    responseString.push_back(86);
    responseString.push_back(55);
    responseString.push_back(46);
    responseString.push_back(50);
    responseString.push_back(32);
    responseString.push_back(0);
    const int len = responseString.size()-2;
    char foo[4];
    std::list<int>::iterator it = responseString.begin();
    it++;
    int i=0;
    do {
        foo[i] = *it;
        it++;
        i++;
    } while (it != responseString.end());
    std::cout << "Characters: " << (std::string) foo << std::endl;
    QString s = foo;
    QStringList xList = s.split('.');
    for (int i = 0; i<xList.length(); i++) {
        bool ok = true;
        int hex = xList.at(i).toInt(&ok, 10);
        if (!ok)
            xList[i] = "0";
    }

    std::tuple<int, int, int> magstimResponse;
    if (xList.length() == 3)
        magstimResponse = std::make_tuple(xList.at(0).toInt(), xList.at(1).toInt(), xList.at(2).toInt());
    if (xList.length() == 2)
        magstimResponse = std::make_tuple(xList.at(0).toInt(), xList.at(1).toInt(), 0);
    if (xList.length() == 1)
        magstimResponse = std::make_tuple(xList.at(0).toInt(), 0, 0);


    std::cout << "Tuple: " << std::get<0>(magstimResponse) << "." << std::get<1>(magstimResponse) << "." << std::get<2>(magstimResponse) << std::endl;
    return 0;
}
