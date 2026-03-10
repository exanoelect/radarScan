#include "NetworkMonitorQt.h"

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <net/if.h>
#include <arpa/inet.h>

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
            &NetworkMonitorQt::handleNetlinkEvent);
}

NetworkMonitorQt::~NetworkMonitorQt()
{
    if (notifier)
        notifier->deleteLater();

    if (netlinkFd >= 0)
        close(netlinkFd);
}

void NetworkMonitorQt::handleNetlinkEvent()
{
    char buffer[4096];

    int len = recv(netlinkFd, buffer, sizeof(buffer), 0);
    if (len <= 0)
        return;

    parseMessages(buffer, len);
}

void NetworkMonitorQt::parseMessages(char *buffer, int len)
{
    for (nlmsghdr *nh = (nlmsghdr*)buffer;
         NLMSG_OK(nh, len);
         nh = NLMSG_NEXT(nh, len))
    {
        switch (nh->nlmsg_type)
        {
        case RTM_NEWLINK:
            handleLinkEvent(nh);
            break;

        case RTM_NEWADDR:
            handleAddrEvent(nh);
            break;

        default:
            break;
        }
    }
}

void NetworkMonitorQt::handleLinkEvent(struct nlmsghdr *nh)
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

