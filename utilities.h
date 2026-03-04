#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QProcess>

#ifdef Q_OS_LINUX
    #include <sys/ioctl.h> //Header ini mendefinisikan fungsi ioctl, yang merupakan singkatan dari input/output control. Fungsi ini digunakan untuk melakukan operasi tingkat rendah pada file descriptor (biasanya terkait dengan perangkat keras, seperti antarmuka jaringan).
    #include <net/if.h> //Header ini berisi definisi dan struktur datas yang digunakan untuk berinteraksi dengan antarmuka jaringan di sistem operasi berbasis Unix/Linux.
#endif

#include <unistd.h> //Header ini berisi deklarasi untuk berbagai fungsi sistem standar di Unix, seperti operasi file, manajemen proses, dan manipulasi direktori.
#include <QCursor>
#include <qcursor.h>
#include <QMessageBox>
#include <QSpacerItem>
#include <QThread>
#include <QRegularExpression>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

struct WifiAP
{
    QString ssid;
    int channel;
    int freqMHz;
    QString band;      // "2.4GHz" atau "5GHz"
    int rateMbps;
    int signalPercent;
    int signalDbm;
    QString security;
};


class utilities : public QObject
{
    Q_OBJECT
public:
    explicit utilities(QObject *parent = nullptr);

signals:
    void wifiListReady(QStringList ssidList);
    void wifiListReadyComplete(QList<WifiAP> wifiList);

    void ssidReady(QString ssid);
    void wifiConnectResult(bool success,
                           QString ssid,
                           QString ipAddress);
    void wifiRadioChanged(bool success);
    void wifiForgetResult(bool success, QString ssid, QString message);
    void wifiCurrentInfoReady(QJsonObject info);
    void wifiDisconnectResult(bool success,
                               QString ssid,
                               QString message);
    //wifiDisconnectResult(bool ok, QString ssid, QString output);

public slots:
    void nmcliGetSSID();
    void nmcliGetWifiListSSid();
    void nmcliGetWifiListComplete();
    void nmcliGetCurrentWifiInfo();

    void nmcliWifiOn();
    void nmcliWifiOff();
    void nmcliConnectToWiFi(const QString &ssid, const QString &password);
    void nmcliDisconnectCurrentWifi();
    void nmcliForgetConnection(const QString &ssid);

    bool rpiRestart();
    bool rpiShutdown();

private:
    void runNmcli(QStringList args,std::function<void(bool, QString)> callback);
    void runNmcliBash(QStringList args,std::function<void(bool, QString)> callback);

    QList<WifiAP> parseNmcliOutput(const QString &output);
};

#endif // UTILITIES_H
