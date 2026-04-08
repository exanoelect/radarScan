#include "NetworkMonitorQt.h"

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if.h>

NetworkMonitorQt::NetworkMonitorQt(const QString& interface, QObject *parent)
    : QObject(parent), interfaceName(interface)
{
    netlinkFd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    if (netlinkFd < 0)
        qFatal("Failed to create netlink socket");

    sockaddr_nl addr{};
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    if (bind(netlinkFd, (sockaddr*)&addr, sizeof(addr)) < 0)
        qFatal("Failed to bind netlink socket");

    notifier = new QSocketNotifier(netlinkFd, QSocketNotifier::Read, this);

    connect(notifier,
            &QSocketNotifier::activated,
            this,
            &NetworkMonitorQt::handleLinkEvent);

    healthTimer = new QTimer(this);

    connect(healthTimer, &QTimer::timeout, this, [=]() {
        int rssi = getRSSI(interfaceName);

        double loss = 0;
        double latency = 0;
        runPingTest("8.8.8.8", loss, latency);

        emit wifiHealthUpdated(rssi, latency, loss);
    });

    healthTimer->start(2000);
}

//---------------------------------------------------------------------------
NetworkMonitorQt::~NetworkMonitorQt()
{
    if (notifier)
        notifier->deleteLater();

    if (netlinkFd >= 0)
        close(netlinkFd);
}

//---------------------------------------------------------------------------
/*
void NetworkMonitorQt::handleLinkMessages(struct nlmsghdr *nh)
{
    char buffer[4096];

    int len = recv(netlinkFd, buffer, sizeof(buffer), 0);
    if (len <= 0)
        return;

    parseMessages(buffer, len);

    ifinfomsg *ifi = (ifinfomsg*)NLMSG_DATA(nh);

    bool carrier = ifi->ifi_flags & IFF_LOWER_UP;
    bool up = ifi->ifi_flags & IFF_UP;

    if (!carrier){
        emit wifiSignalLost();
    }

    if (!up){
        emit networkInterfaceDown();
        return;
    }
}
*/

//---------------------------------------------------------------------------
void NetworkMonitorQt::parseMessages(char *buffer, int len)
{
    for (nlmsghdr *nh = (nlmsghdr*)buffer;
         NLMSG_OK(nh, len);
         nh = NLMSG_NEXT(nh, len))
    {
        switch (nh->nlmsg_type)
        {
        case RTM_NEWLINK:
            //handleLinkEvent(nh);
            handleLinkMessages(nh);
            break;

        case RTM_NEWADDR:
            handleAddrEvent(nh);
            break;

        default:
            break;
        }
    }
}

//---------------------------------------------------------------------------
void NetworkMonitorQt::handleLinkEvent()//struct nlmsghdr *nh)
{
    char buffer[4096];

    int len = recv(netlinkFd, buffer, sizeof(buffer), 0);
    if (len <= 0)
        return;

    parseMessages(buffer, len);


}

//---------------------------------------------------------------------------
void NetworkMonitorQt::handleLinkMessages(nlmsghdr *nh)
{
        ifinfomsg *ifi = (ifinfomsg*)NLMSG_DATA(nh);

        int attrlen = nh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi));
        rtattr *attr = (rtattr*)IFLA_RTA(ifi);

        QString ifname;

        for (; RTA_OK(attr, attrlen); attr = RTA_NEXT(attr, attrlen))
        {
            if (attr->rta_type == IFLA_IFNAME)
            {
                ifname = QString((char*)RTA_DATA(attr));
            }
        }

        if (ifname != interfaceName)
            return;

        bool running = ifi->ifi_flags & IFF_RUNNING;
        bool up = ifi->ifi_flags & IFF_UP;

        if (!up)
        {
            emit networkInterfaceDown();
            return;
        }

        if (running != lastLinkState)
        {
            lastLinkState = running;

            if (running)
            {
                emit wifiConnected();
            }
            else
            {
                emit wifiSignalLost();
                emit wifiDisconnected();
            }
        }
}

//---------------------------------------------------------------------------
void NetworkMonitorQt::handleAddrEvent(struct nlmsghdr *nh)
{
    ifaddrmsg *ifa = (ifaddrmsg*)NLMSG_DATA(nh);

    char ifname[IF_NAMESIZE];
    if_indextoname(ifa->ifa_index, ifname);

    if (QString(ifname) != interfaceName)
        return;

    int attrlen = nh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa));
    rtattr *attr = IFA_RTA(ifa);

    for (; RTA_OK(attr, attrlen); attr = RTA_NEXT(attr, attrlen))
    {
        if (attr->rta_type == IFA_LOCAL)
        {
            char ip[INET_ADDRSTRLEN];

            inet_ntop(AF_INET,
                      RTA_DATA(attr),
                      ip,
                      sizeof(ip));

            emit ipAddressChanged(QString(ip));
        }
    }
}

//---------------------------------------------------------------------------
int NetworkMonitorQt::getRSSI(const QString &iface)
{
    QProcess p;
    p.start("iw dev " + iface + " link");
    p.waitForFinished();

    QString out = p.readAllStandardOutput();

    QRegularExpression re("signal: (-?\\d+) dBm");
    auto match = re.match(out);

    if (match.hasMatch())
        return match.captured(1).toInt();

    return -1000; // invalid

}

//---------------------------------------------------------------------------
void NetworkMonitorQt::runPingTest(const QString &ip, double &loss, double &latency)
{
    QProcess p;
    p.start("ping -c 3 " + ip);
    p.waitForFinished();

    QString out = p.readAllStandardOutput();

    QRegularExpression lossRe("(\\d+)% packet loss");
    auto l = lossRe.match(out);
    if (l.hasMatch())
        loss = l.captured(1).toDouble();

    QRegularExpression latRe("rtt min/avg/max/mdev = [^/]*/([^/]*)/");
    auto m = latRe.match(out);
    if (m.hasMatch())
        latency = m.captured(1).toDouble();

}

//---------------------------------------------------------------------------
bool NetworkMonitorQt::checkFirmwareHealth()
{
    QProcess p;
    p.start("dmesg | grep brcmfmac");
    p.waitForFinished();

    QString out = p.readAllStandardOutput();

    if (out.contains("error") || out.contains("fail"))
        return false;

    return true;

}

