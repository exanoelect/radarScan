#ifndef NETWORKMONITORQT_H
#define NETWORKMONITORQT_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QTimer>
#include <QProcess>
#include <QRegularExpression>

#ifdef Q_OS_LINUX
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

class NetworkMonitorQt : public QObject
{
    Q_OBJECT

public:
    explicit NetworkMonitorQt(const QString& interface = "wlan0",
                              QObject *parent = nullptr);

    ~NetworkMonitorQt();

signals:
    void wifiConnected();
    void wifiDisconnected();
    void wifiSignalLost();
    void networkInterfaceDown();
    void ipAddressChanged(QString newIp);

    void wifiHealthUpdated(int rssi, double latency, double packetLoss);

private slots:
    void handleLinkEvent();//struct nlmsghdr *nh);
    void handleLinkMessages(struct nlmsghdr *nh);


private:
    int netlinkFd = -1;
    QSocketNotifier *notifier = nullptr;
    QString interfaceName;

    bool lastLinkState = false;

    void parseMessages(char *buffer, int len);
    void handleAddrEvent(struct nlmsghdr *nh);

    //Add on
    int getRSSI(const QString &iface);
    void runPingTest(const QString &ip,
                     double &loss,
                     double &latency);
    bool checkFirmwareHealth();

    QTimer *healthTimer;

};

#endif
