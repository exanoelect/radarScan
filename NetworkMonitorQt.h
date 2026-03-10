#ifndef NETWORKMONITORQT_H
#define NETWORKMONITORQT_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

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

private slots:
    void handleNetlinkEvent();

private:
    int netlinkFd = -1;
    QSocketNotifier *notifier = nullptr;
    QString interfaceName;

    bool lastLinkState = false;

    void parseMessages(char *buffer, int len);
    void handleLinkEvent(struct nlmsghdr *nh);
    void handleAddrEvent(struct nlmsghdr *nh);
};

#endif
