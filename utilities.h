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

class utilities : public QObject
{
    Q_OBJECT
public:
    explicit utilities(QObject *parent = nullptr);

signals:
public slots:
    QString nmcliGetSSID();
    QStringList nmcliGetWifiList();

    void nmcliWifiOn();
    void nmcliWifiOff();
    bool nmcliConnectToWiFi(const QString &ssid, const QString &password);
    bool nmcliForgetConnection(const QString &ssid);

    bool rpiRestart();
    bool rpiShutdown();
};

#endif // UTILITIES_H
