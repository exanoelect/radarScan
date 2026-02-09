#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>

class ConfigManager
{
public:
    static QString getServerIp();
    static int getServerPort();
};

#endif // CONFIGMANAGER_H
