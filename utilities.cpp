#include "utilities.h"

utilities::utilities(QObject *parent)
    : QObject{parent}
{

}

//------------------------------------------------------------------------------------------------------------------------
QString utilities::nmcliGetSSID()
{
    QProcess process;

    process.start("nmcli", {"-t", "-f", "NAME", "connection", "show", "--active"});

    if (!process.waitForFinished(3000))
        return {};

    const QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();

    return output.section('\n', 0, 0);  // ambil baris pertama

}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliGetWifiList()
{
    QProcess *process = new QProcess(this);

    connect(process, &QProcess::finished,
            this, [this, process](int, QProcess::ExitStatus) {

        QString output =
            QString::fromUtf8(process->readAllStandardOutput()).trimmed();

        QStringList ssidList =
            output.split('\n', Qt::SkipEmptyParts);

        ssidList.removeAll("");

        emit wifiListReady(ssidList);

        process->deleteLater();
    });

    //nmcli -t -f SSID device wifi list
    process->start("nmcli",
                   QStringList()
                   << "-t" << "-f" << "SSID"
                   << "device" << "wifi" << "list");
    //               << "--rescan" << "no");
}


//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliWifiOn()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "nmcli radio wifi on");
    process.waitForFinished();
    qDebug() << "WiFi disabled: " << process.readAllStandardOutput();
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliWifiOff()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "nmcli radio wifi off");
    process.waitForFinished();
    qDebug() << "WiFi disabled: " << process.readAllStandardOutput();
}

//------------------------------------------------------------------------------------------------------------------------
bool utilities::nmcliConnectToWiFi(const QString &ssid, const QString &password)
{
    QProcess process;
    process.start("nmcli",
                  {"device","wifi","connect", ssid, "password", password});

    if (!process.waitForStarted())
        return false;

    if (!process.waitForFinished(15000))
        return false;

    qDebug() << "STDOUT:" << process.readAllStandardOutput();
    qDebug() << "STDERR:" << process.readAllStandardError();

    return (process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0);
}

//------------------------------------------------------------------------------------------------------------------------
bool utilities::nmcliForgetConnection(const QString &ssid)
{
    QProcess process;

    process.start("nmcli",
                   QStringList()
                       << "connection"
                       << "delete"
                       << ssid);

    if (!process.waitForStarted())
         return false;

    if (!process.waitForFinished(5000))
         return false;

    qDebug() << "STDOUT:" << process.readAllStandardOutput();
    qDebug() << "STDERR:" << process.readAllStandardError();

    return (process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0);
}

//------------------------------------------------------------------------------------------------------------------------
bool utilities::rpiRestart()
{
    QProcess process;

    process.start("systemctl", QStringList() << "reboot");

    if (!process.waitForStarted())
        return false;

    if (!process.waitForFinished(3000))
        return false;

    return (process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0);
}

//------------------------------------------------------------------------------------------------------------------------
bool utilities::rpiShutdown()
{
    QProcess process;

    process.start("systemctl", {"poweroff"});

    if (!process.waitForStarted())
        return false;

    if (!process.waitForFinished(3000))
        return false;

    return (process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0);
}

