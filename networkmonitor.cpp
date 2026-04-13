#include "networkmonitor.h"
#include <QDebug>

#include <net/if.h>
#include <unistd.h>

#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <netlink/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

//---------------------------------------------------------------------------------------
NetworkMonitor::NetworkMonitor(const QString& iface, QObject *parent)
    : QObject(parent), m_iface(iface)
{
}

//---------------------------------------------------------------------------------------
NetworkMonitor::~NetworkMonitor()
{
    stop();
    cleanupNetlink();
}

//---------------------------------------------------------------------------------------
bool NetworkMonitor::start()
{
    initNetlink();
    if (!m_sock) return false;

    int fd = nl_socket_get_fd(m_sock);
    if (fd < 0) return false;

    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated,
            this, &NetworkMonitor::handleNlReadable);

    return true;
}

//---------------------------------------------------------------------------------------
void NetworkMonitor::stop()
{
    if (m_notifier) {
        m_notifier->setEnabled(false);
        delete m_notifier;
        m_notifier = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void NetworkMonitor::initNetlink()
{
    if (m_sock) return;

    m_sock = nl_socket_alloc();
    if (!m_sock) {
        qDebug() << "nl_socket_alloc failed";
        return;
    }

    if (genl_connect(m_sock)) {
        qDebug() << "genl_connect failed";
        cleanupNetlink();
        return;
    }

    m_driverId = genl_ctrl_resolve(m_sock, "nl80211");
    if (m_driverId < 0) {
        qDebug() << "genl_ctrl_resolve failed";
        cleanupNetlink();
        return;
    }

    m_ifindex = if_nametoindex(m_iface.toStdString().c_str());
    if (m_ifindex == 0) {
        qDebug() << "invalid iface";
        cleanupNetlink();
        return;
    }

    // Subscribe ke event groups
    //int mcid;

    //mcid = genl_ctrl_resolve_grp(m_sock, "nl80211", "mlme");
    //if (mcid >= 0) nl_socket_add_membership(m_sock, mcid);

    //mcid = genl_ctrl_resolve_grp(m_sock, "nl80211", "station");
    //if (mcid >= 0) nl_socket_add_membership(m_sock, mcid);

    nl_socket_modify_cb(m_sock, NL_CB_VALID, NL_CB_CUSTOM,
                        &NetworkMonitor::eventCallback, this);

    nl_socket_disable_seq_check(m_sock);
    nl_socket_modify_cb(m_sock, NL_CB_VALID, NL_CB_CUSTOM,
                        &NetworkMonitor::eventCallback, this);
}

//---------------------------------------------------------------------------------------
void NetworkMonitor::cleanupNetlink()
{
    if (m_sock) {
        nl_socket_free(m_sock);
        m_sock = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void NetworkMonitor::handleNlReadable()
{
    if (!m_sock) return;

    nl_socket_modify_cb(m_sock, NL_CB_VALID, NL_CB_CUSTOM,
                        &NetworkMonitor::eventCallback, this);

    nl_recvmsgs_default(m_sock);

    qDebug() << "Netlink readable triggered"; // <--- tambahin ini
    qDebug() << "Netlink FD:" << nl_socket_get_fd(m_sock);
    //qDebug() << "mlme group:" << mcid;
    qDebug() << "station group:" << genl_ctrl_resolve_grp(m_sock, "nl80211", "station");
}

//---------------------------------------------------------------------------------------
int NetworkMonitor::eventCallback(struct nl_msg *msg, void *arg)
{
    auto *self = static_cast<NetworkMonitor*>(arg);

    struct genlmsghdr *gnlh =
        (genlmsghdr*) nlmsg_data(nlmsg_hdr(msg));

    switch (gnlh->cmd) {

    case NL80211_CMD_CONNECT:
    case NL80211_CMD_ROAM:
        self->m_connected = true;
        emit self->connectionChanged(true);
        self->queryRssi();
        break;

    case NL80211_CMD_DISCONNECT:
        self->m_connected = false;
        self->m_rssi = -1000;
        emit self->connectionChanged(false);
        emit self->rssiChanged(self->m_rssi);
        break;

    case NL80211_CMD_NEW_STATION:
    case NL80211_CMD_DEL_STATION:
        self->queryRssi();
        break;

    default:
        break;
    }

    return NL_SKIP;
}

//---------------------------------------------------------------------------------------
void NetworkMonitor::queryRssi()
{
    if (!m_sock) return;

    struct nl_msg *msg = nlmsg_alloc();
    if (!msg) return;

    genlmsg_put(msg, 0, 0, m_driverId, 0, 0,
                NL80211_CMD_GET_STATION, 0);

    nla_put_u32(msg, NL80211_ATTR_IFINDEX, m_ifindex);

    int newRssi = -1000;

    nl_socket_modify_cb(m_sock, NL_CB_VALID, NL_CB_CUSTOM,
                        &NetworkMonitor::rssiCallback, &newRssi);

    if (nl_send_auto(m_sock, msg) >= 0) {
        nl_recvmsgs_default(m_sock);
    }

    nlmsg_free(msg);

    if (newRssi != m_rssi) {
        m_rssi = newRssi;
        emit rssiChanged(m_rssi);
    }
}

//---------------------------------------------------------------------------------------
int NetworkMonitor::rssiCallback(struct nl_msg *msg, void *arg)
{
    int *rssi = static_cast<int*>(arg);

    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *gnlh =
        (genlmsghdr*) nlmsg_data(nlmsg_hdr(msg));

    nla_parse(tb, NL80211_ATTR_MAX,
              genlmsg_attrdata(gnlh, 0),
              genlmsg_attrlen(gnlh, 0),
              nullptr);

    if (tb[NL80211_ATTR_STA_INFO]) {
        struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];

        nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
                         tb[NL80211_ATTR_STA_INFO],
                         nullptr);

        if (sinfo[NL80211_STA_INFO_SIGNAL]) {
            int8_t signal =
                (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);

            *rssi = signal;
        }
    }

    return NL_SKIP;
}
