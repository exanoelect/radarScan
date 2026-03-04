#include "utilities.h"


utilities::utilities(QObject *parent)
    : QObject{parent}
{
    initWlanMonitor();
}

utilities::~utilities(){

}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliGetSSID()
{
    runNmcli(
                //nmcli -t -f connection show --active
        {"-t", "-f", "NAME", "connection", "show", "--active"},
        [this](bool success, QString output) {

            if (!success) {
                emit ssidReady(QString());
                return;
            }

            QString ssid = output.trimmed().section('\n', 0, 0);
            emit ssidReady(ssid);
        });
}


//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliGetWifiListSSid()
{
    runNmcli(
        //nmcli -t -f SSID device wifi list
        {"-t", "-f", "SSID", "device", "wifi", "list"},
        [this](bool success, QString output) {
            if (!success) {
                emit wifiListReady(QStringList());
                return;
            }

            QStringList list =
                output.trimmed().split('\n', Qt::SkipEmptyParts);

            list.removeAll("");

            emit wifiListReady(list);
        }
    );
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliGetWifiListComplete()
{
    QStringList args;
    args << "-t"
         << "-f"
         << "SSID,CHAN,FREQ,RATE,SIGNAL,SECURITY"
         << "device"
         << "wifi"
         << "list"
         << "--rescan"
         << "yes";

    runNmcli(args, [this](bool success, QString output)
    {
        if (!success)
        {
            emit wifiListReady({});
            return;
        }

        QList<WifiAP> wifiList = parseNmcliOutput(output);

        emit wifiListReadyComplete(wifiList);
    });
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliGetCurrentWifiInfo()
{
    QString script = R"SCRIPT(
    IF=wlan0
    SSID=$(nmcli -t -f ACTIVE,SSID dev wifi 2>/dev/null | awk -F: '/^yes/ {print $2}')
    SIGNAL=$(nmcli -t -f IN-USE,SIGNAL dev wifi 2>/dev/null | awk -F: '/^\*/ {print $2}')
    IP=$(nmcli -t -f IP4.ADDRESS device show $IF 2>/dev/null | head -n1 | cut -d: -f2 | cut -d/ -f1)
    CONN=$(nmcli -t -f GENERAL.CONNECTION device show $IF 2>/dev/null | cut -d: -f2)

    TS=0
    if [ -n "$CONN" ]; then
        TS=$(nmcli -t -f connection.timestamp connection show "$CONN" 2>/dev/null)
    fi

    NOW=$(date +%s)

    if [ -z "$SIGNAL" ]; then
        SIGNAL=0
    fi

    if [ -n "$TS" ]; then
        UPTIME=$((NOW-TS))
    else
        UPTIME=0
    fi

    printf '{"ssid":"%s","signal_dbm":%d,"ip":"%s","uptime_sec":%d}' \
    "$SSID" "$((SIGNAL/2-100))" "$IP" "$UPTIME"
    )SCRIPT";

    QStringList args;
    args << "-c" << script;

    runNmcliBash({"-c", script}, [this](bool success, QString output)
    {
        if (!success)
        {
            qDebug() << "bash exe fail";
            qDebug() << "STDOUT:" << output;
           // qDebug() << "STDERR:" << error;
           // qDebug() << "ExitCode:" << exitCode;

            emit wifiCurrentInfoReady(QJsonObject());
            return;
        }

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(output.trimmed().toUtf8(), &err);

        if (err.error != QJsonParseError::NoError || !doc.isObject())
        {
            qDebug() << "JSON parse error";
            emit wifiCurrentInfoReady(QJsonObject());
            return;
        }

        qDebug() << "bash exe ok";
        emit wifiCurrentInfoReady(doc.object());
    });
}


//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliWifiOn()
{    
    runNmcli(
        {"radio", "wifi", "on"},
        [this](bool success, QString output) {
            if (success) {
                qDebug() << "WiFi Enabled";
            }else{
                qDebug() << "Failed enable WiFi:" << output;
            }

            emit wifiRadioChanged(success);
        }
    );
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliWifiOff()
{
    runNmcli(
        {"radio", "wifi", "off"},
        [this](bool success, QString output) {

            if (success) {
                qDebug() << "WiFi OFF";
            }else{
                qDebug() << "Failed Disable  WiFi:" << output;
            }

            // Optional: bisa emit signal kalau perlu
            emit wifiRadioChanged(false);
        });
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliConnectToWiFi(const QString &ssid,
                                   const QString &password)
{
    emit wifiConnectProgress(0, "connecting");

    runNmcli(
        {"device","wifi","connect", ssid, "password", password},
        [this, ssid](bool success, QString output) {

            if (!success) {
                emit wifiConnectResult(false, ssid, "", "");
                return;
            }

            // Kalau connect sukses, ambil IP + Gateway sekaligus
            runNmcli(
                {"-g", "IP4.ADDRESS,IP4.GATEWAY",
                 "device", "show", "wlan0"},
                [this, ssid](bool netSuccess, QString netOutput) {

                    QString ipAddress;
                    QString gateway;

                    if (netSuccess) {

                        QStringList lines =
                            netOutput.trimmed()
                                     .split('\n', Qt::SkipEmptyParts);

                        // Biasanya:
                        // 0 = 192.168.1.23/24
                        // 1 = 192.168.1.1

                        if (lines.size() > 0) {
                            ipAddress =
                                lines[0].section('/', 0, 0);
                        }

                        if (lines.size() > 1) {
                            gateway = lines[1];
                        }
                    }

                    emit wifiConnectResult(true,
                                           ssid,
                                           ipAddress,
                                           gateway);
                });
        });
}


//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliDisconnectCurrentWifi()
{
    // STEP 1: Ambil SSID aktif
    runNmcli(
        {"-t", "-f", "GENERAL.CONNECTION", "device", "show", "wlan0"},
        [this](bool ok, QString output) {

            if (!ok) {
                emit wifiDisconnectResult(false, "", output);
                return;
            }

            // Output contoh:
            // GENERAL.CONNECTION:Parametrik 5G-01

            QString line = output.trimmed();
            QString ssid = line.section(':', 1).trimmed();

            if (ssid.isEmpty() || ssid == "--") {
                emit wifiDisconnectResult(false, "", "No active WiFi");
                return;
            }

            qDebug() << "Active SSID:" << ssid;

            // STEP 2: Disconnect SSID tersebut
            runNmcli(
                {"connection", "down", ssid},
                [this, ssid](bool ok2, QString output2) {

                    emit wifiDisconnectResult(ok2, ssid, output2);
                }
            );
        }
    );
}


//------------------------------------------------------------------------------------------------------------------------
void utilities::nmcliForgetConnection(const QString &ssid)
{
    runNmcli(
        {"connection", "delete", ssid},
        [this, ssid](bool success, QString output) {

            qDebug() << "Forget WiFi result:" << output;

            emit wifiForgetResult(success, ssid, output);
        }
    );
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

//------------------------------------------------------------------------------------------------------------------------
void utilities::onDeviceStateChanged(uint newState,
                                     uint oldState,
                                     uint reason)
{
    Q_UNUSED(oldState)
    Q_UNUSED(reason)

    QString text = deviceStateToString(newState);

    emit wifiConnectProgress(newState, text);

    if (newState == 100) {
        qDebug() << "WiFi Fully Activated";
    }

    if (newState == 120) {
        qDebug() << "WiFi Failed";
    }
}


//------------------------------------------------------------------------------------------------------------------------
void utilities::runNmcli(QStringList args,
                         std::function<void(bool, QString)> callback)
{
    QProcess *process = new QProcess(this);

    connect(process, &QProcess::finished,
            this, [process, callback](int exitCode,
                                      QProcess::ExitStatus status) {

        bool success = (status == QProcess::NormalExit && exitCode == 0);
        QString stdoutStr = QString::fromUtf8(process->readAllStandardOutput());
        QString stderrStr = QString::fromUtf8(process->readAllStandardError());

        QString message = success ? stdoutStr : stderrStr;
        callback(success, message);
        process->deleteLater();
    });

    connect(process, &QProcess::errorOccurred,
            this, [process, callback](QProcess::ProcessError) {

        callback(false, "Process error");
        process->deleteLater();
    });

    process->start("nmcli", args);

    QTimer::singleShot(15000, process, [process]() {
        if (process->state() != QProcess::NotRunning)
            process->kill();
    });
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::runNmcliBash(QStringList args, std::function<void (bool, QString)> callback)
{
    QProcess *process = new QProcess(this);

    connect(process, &QProcess::finished,
            this, [process, callback](int exitCode,
                                      QProcess::ExitStatus status) {

        bool success = (status == QProcess::NormalExit && exitCode == 0);
        QString stdoutStr = QString::fromUtf8(process->readAllStandardOutput());
        QString stderrStr = QString::fromUtf8(process->readAllStandardError());

        QString message = success ? stdoutStr : stderrStr;
        callback(success, message);
        process->deleteLater();
    });

    connect(process, &QProcess::errorOccurred,
            this, [process, callback](QProcess::ProcessError) {

        callback(false, "Process error");
        process->deleteLater();
    });

    process->start("bash", args);

    QTimer::singleShot(15000, process, [process]() {
        if (process->state() != QProcess::NotRunning)
            process->kill();
    });
}

//------------------------------------------------------------------------------------------------------------------------
QList<WifiAP> utilities::parseNmcliOutput(const QString &output)
{
    QList<WifiAP> list;

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines)
    {
        QStringList parts = line.split(':');

        // Pastikan field lengkap
        if (parts.size() < 6)
            continue;

        WifiAP ap;

        ap.ssid = parts[0].trimmed();   // bisa kosong
        ap.channel = parts[1].toInt();

        // remove " MHz"
        QString freqStr = parts[2];
        freqStr.remove(" MHz");
        ap.freqMHz = freqStr.toInt();

        // tentukan band
        ap.band = (ap.freqMHz < 3000) ? "2.4GHz" : "5GHz";

        // remove " Mbit/s"
        QString rateStr = parts[3];
        rateStr.remove(" Mbit/s");
        ap.rateMbps = rateStr.toInt();

        ap.signalPercent = parts[4].toInt();

        // konversi ke dBm (estimasi)
        ap.signalDbm = (ap.signalPercent / 2) - 100;

        ap.security = parts[5].trimmed();

        list.append(ap);
    }

    return list;
}

//------------------------------------------------------------------------------------------------------------------------
QString utilities::deviceStateToString(uint state)
{
    switch (state) {
    case 20: return "Unavailable";
    case 30: return "Disconnected";
    case 40: return "Preparing";
    case 50: return "Configuring";
    case 60: return "Need Auth";
    case 70: return "IP Config";
    case 80: return "IP Check";
    case 90: return "Secondaries";
    case 100: return "Activated";
    case 120: return "Failed";
    default: return "Unknown";
    }
}

//------------------------------------------------------------------------------------------------------------------------
void utilities::initWlanMonitor()
{
    QDBusInterface nm(
        "org.freedesktop.NetworkManager",
        "/org/freedesktop/NetworkManager",
        "org.freedesktop.NetworkManager",
        QDBusConnection::systemBus());

    QDBusReply<QList<QDBusObjectPath>> reply =
        nm.call("GetDevices");

    if (!reply.isValid())
        return;

    for (const QDBusObjectPath &path : reply.value()) {

        QDBusInterface dev(
            "org.freedesktop.NetworkManager",
            path.path(),
            "org.freedesktop.NetworkManager.Device",
            QDBusConnection::systemBus());

        QString iface =
            dev.property("Interface").toString();

        if (iface == "wlan0") {

            wlanDevicePath = path.path();

            QDBusConnection::systemBus().connect(
                "org.freedesktop.NetworkManager",
                wlanDevicePath,
                "org.freedesktop.NetworkManager.Device",
                "StateChanged",
                this,
                SLOT(onDeviceStateChanged(uint,uint,uint)));

            qDebug() << "Monitoring wlan0 at:"
                     << wlanDevicePath;

            break;
        }
    }
}


