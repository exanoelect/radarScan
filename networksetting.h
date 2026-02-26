#ifndef NETWORKSETTING_H
#define NETWORKSETTING_H

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
//#include <QRegExp>
#include <QRegularExpression>

//float timbanganTerraNull;
//bool terraClicked;

//bool addWifiNetwork(const QString& ssid, const QString& password);   // Fungsi untuk menambahkan SSID dan password ke file wpa_supplicant.conf
//bool updateWifiConfig(const QString& ssid, const QString& password); // Fungsi untuk menambahkan atau memperbarui SSID dan password di file wpa_supplicant.conf
bool update_wpa_supplicant(const QString& ssid, const QString& password); // Fungsi untuk menambahkan atau memperbarui SSID dan password di file wpa_supplicant.conf

//SSid wifi
QString getCurrentSSID();
bool wifiDown();
bool wifiUp();

bool wpa_supplicantStop();
bool wpa_supplicantStart();

//NMCLI config
bool dhcpcd_stop();
bool dhcpcd_disable();
bool NetworkManagerEnable();
bool NetworkManagerRestart();
bool NetworkManagerStart();
bool nmcliRescanWifi();
bool nmcliSleep();
QStringList nmcliGetWifiList();
bool nmcliConnectToWiFi(const QString &ssid, const QString &password);
QString nmcliGetSSID();
void nmcliWifiOn();
void nmcliWifiOff();
bool nmcliInit();
bool pingGoogle();

#endif // NETWORKSETTING_H

