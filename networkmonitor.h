#pragma once

#include <QObject>
#include <QSocketNotifier>

struct nl_sock;

class NetworkMonitor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMonitor(const QString& iface = "wlan0", QObject *parent = nullptr);
    ~NetworkMonitor();

    bool start();
    void stop();

    int rssi() const { return m_rssi; }
    bool isConnected() const { return m_connected; }

signals:
    void rssiChanged(int rssi);
    void connectionChanged(bool connected);

private:
    void initNetlink();
    void cleanupNetlink();

    void handleNlReadable();      // dipanggil QSocketNotifier
    void queryRssi();             // dipanggil saat ada event

    static int eventCallback(struct nl_msg *msg, void *arg);
    static int rssiCallback(struct nl_msg *msg, void *arg);

private:
    QString m_iface;
    int m_ifindex = 0;

    nl_sock *m_sock = nullptr;
    int m_driverId = -1;

    QSocketNotifier *m_notifier = nullptr;

    int m_rssi = -1000;
    bool m_connected = false;
};
