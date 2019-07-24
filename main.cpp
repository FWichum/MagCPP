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

    std::list<int> responseString = {38, 50, 57, 48, 12, 11, 11};

    std::string str(responseString.begin(), responseString.end());
    QString qs =  QString::fromUtf8(str.data(), str.size());
    std::cout << "    " << qs.mid(1,2).toStdString();
    return 0;
}
