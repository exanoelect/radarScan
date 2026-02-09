#include "configmanager.h"
#include <QSettings>
#include <QCoreApplication>

QString ConfigManager::getServerIp()
{
    //QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
#ifdef PLATFORM_LINUX
    QString configPath = "/home/pi/app/config.ini";
#else
    QString configPath = "/Volumes/DATA/app/config.ini";
#endif

    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value("Server/ip", "127.0.0.1").toString();
}

int ConfigManager::getServerPort()
{
    //QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
#ifdef PLATFORM_LINUX
    QString configPath = "/home/pi/app/config.ini";
#else
    QString configPath = "/Volumes/DATA/app/config.ini";
#endif
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value("Server/port", 3000).toInt();
}
