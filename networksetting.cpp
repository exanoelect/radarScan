#include <networksetting.h>

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool update_wpa_supplicant(const QString &ssid, const QString &password) {

    if(wpa_supplicantStop()){
        qDebug() << "Success stop wpa_supplicant";
    }else{
        qDebug() << "fail stop wpa_supplicant";
        return false;
    }

    QFile file("/etc/wpa_supplicant/wpa_supplicant.conf");

    // Buka file untuk membaca isinya
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open wpa_supplicant.conf for reading.";
        return false;
    }

    QString newConfig;
    QTextStream in(&file);
    bool inNetworkBlock = false;
    bool ssidBlockFound = false;
    QString currentBlock;
    bool ssidMatched = false;

    while (!in.atEnd()) {
        QString line = in.readLine();

        // Awal blok network
        if (line.contains("network={")) {
            inNetworkBlock = true;
            currentBlock.clear();
        }

        // Simpan konten blok network
        if (inNetworkBlock) {
            currentBlock += line + "\n";

            if (line.trimmed().startsWith("ssid=")) {
                if (line.contains(ssid)) {
                    ssidMatched = true;
                }
            }

            if (line.contains("}")) {
                inNetworkBlock = false;

                // Jika SSID cocok, ubah blok
                if (ssidMatched) {
                    ssidBlockFound = true;
                    currentBlock = "network={\n";
                    currentBlock += "    ssid=\"" + ssid + "\"\n";
                    currentBlock += "    psk=\"" + password + "\"\n";
                    currentBlock += "    priority=3\n";
                    currentBlock += "}\n";
                }

                newConfig += currentBlock;
                currentBlock.clear();
                ssidMatched = false;
                continue;
            }
        } else {
            // Baris di luar blok network
            newConfig += line + "\n";
        }
    }

    file.close();

    // Jika SSID tidak ditemukan, tambahkan entry baru
    if (!ssidBlockFound) {
        newConfig += "\nnetwork={\n";
        newConfig += "    ssid=\"" + ssid + "\"\n";
        newConfig += "    psk=\"" + password + "\"\n";
        newConfig += "    priority=3\n";
        newConfig += "}\n";
    }

    // Tulis ulang file dengan konfigurasi baru
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Unable to open wpa_supplicant.conf for writing.";
        return false;
    }

    QTextStream out(&file);
    out << newConfig;
    out.flush();
    file.close();


     // Restart wpa_supplicant untuk mengaplikasikan perubahan
     if(wpa_supplicantStart()){
        qDebug() << "Wpa supplicant start OK";
     }else{
        qDebug() << "Wpa supplicant start FAIL";
        return false;
     }

     QProcess process;
     //process.start("sudo systemctl restart wpa_supplicant");
     process.start("sudo systemctl restart dhcpcd");
     process.waitForFinished();

     qDebug() << "Wi-Fi configuration updated for SSID:" << ssid;
     return true;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
QString getCurrentSSID()
{
    QProcess process;
     process.start("iwgetid -r");
     process.waitForFinished(); // Wait for the process to finish

     // Get the output
     QString output = process.readAllStandardOutput().trimmed();

     // Display the output
     if (!output.isEmpty()) {
         //label->setText("Connected SSID: " + output);
         qDebug() << "SSID:" << output;
         return output;
     } else {
         //label->setText("No SSID found or error executing command");
         qDebug() << "Error: No SSID found or command failed.";
         return "";
     }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool wifiDown()
{
    QProcess process;
    process.start("sudo ifconfig wlan0 down");
    process.waitForFinished(); // Wait for the process to finish

    // Check the process status
    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "WiFi interface wlan0 has been disabled.";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "Error disabling WiFi interface wlan0.";
       return false;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
/*bool connectNEWSSID(QString ssid, QString password)
{
    //QString ssid = ssidInput->text();
    //  QString password = passwordInput->text();

      if (ssid.isEmpty() || password.isEmpty()) {
          //statusLabel->setText("SSID or Password cannot be empty.");
          return false;
      }

      // Command to connect to new WiFi
      QString command = QString("nmcli dev wifi connect '%1' password '%2'").arg(ssid).arg(password);

      // Execute the command
      QProcess process;
      process.start(command);
      process.waitForFinished();

      QString output = process.readAllStandardOutput().trimmed();
      QString error = process.readAllStandardError().trimmed();

      if (process.exitStatus() == QProcess::NormalExit && error.isEmpty()) {
         // statusLabel->setText("Connected to new SSID: " + ssid);
          qDebug() << "Successfully connected to SSID:" << ssid;
          return true;
      } else {
        //  statusLabel->setText("Failed to connect to SSID: " + ssid);
          qDebug() << "Error:" << error;
          return false;
      }
}
*/

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
//bool updateOrAddToWpaSupplicant(const QString &ssid, const QString &password)
//{
/*    QString filePath = "/etc/wpa_supplicant/wpa_supplicant.conf";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Unable to open wpa_supplicant.conf for reading.";
        return false;
    }

    Naudzubillah, banyak mafia di dalam Pemerintahan. Cukup berat, kudu bisa memilah, mana kawan, mana lawan.
    ASN/Pejabat di Pemerintah sering tampak manis tutur katanya di depan, tp busuk di belakang.
    Kita hanya bisa berikhtiar, toh azab kubur & neraka saqar menunggu mereka para mafia itu. Allahu Akbar!

    QString fileContent = file.readAll();
    file.close();

    QRegularExpression ssidRegex(QString(R"(network\s*{\s*ssid\s*=\s*\"%1\".*?})").arg(ssid),
                                  QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatch match = ssidRegex.match(fileContent);

    if (match.hasMatch()) {
        // Update existing SSID entry
        QString existingEntry = match.captured(0);
        QRegularExpression pskRegex(R"(psk\s*=\s*\".*?\")");
        QString updatedEntry = existingEntry;

        if (pskRegex.match(existingEntry).hasMatch()) {
            updatedEntry = existingEntry.replace(pskRegex, QString("psk=\"%1\"").arg(password));
        } else {
            updatedEntry = existingEntry.trimmed() + QString("\n    psk=\"%1\"\n").arg(password);
        }

        fileContent.replace(existingEntry, updatedEntry);
        qDebug() << "Updated existing SSID entry for:" << ssid;
    } else {
        // Add new SSID entry
        QString newEntry = QString("\nnetwork={\n    ssid=\"%1\"\n    psk=\"%2\"\n    key_mgmt=WPA-PSK\n}\n")
                               .arg(ssid)
                               .arg(password);
        fileContent += newEntry;
        qDebug() << "Added new SSID entry for:" << ssid;
    }

    // Write updated content back to the file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Unable to open wpa_supplicant.conf for writing.";
        return false;
    }

    QTextStream out(&file);
    out << fileContent;
    file.close();

    return true;
    */
//}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool wifiUp()
{
    QProcess process;
    process.start("sudo ifconfig wlan0 up");
    process.waitForFinished(); // Wait for the process to finish

    // Check the process status
    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "WiFi interface wlan0 has been disabled.";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "Error disabling WiFi interface wlan0.";
       return false;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool wpa_supplicantStop()
{
    QProcess process;
    process.start("sudo systemctl stop wpa_supplicant");
    process.waitForFinished(); // Wait for the process to finish

    // Check the process status
    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "WiFi interface wlan0 has been disabled.";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "Error disabling WiFi interface wlan0.";
       return false;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool wpa_supplicantStart()
{
    QProcess process;
    process.start("sudo systemctl start wpa_supplicant");
    process.waitForFinished(); // Wait for the process to finish

    // Check the process status
    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "WiFi interface wlan0 has been enabled.";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "Error enabling WiFi interface wlan0.";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool dhcpcd_stop()
{
    QProcess process;
    process.start("sudo systemctl stop dhcpcd");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    qDebug() << "sudo systemctl stop dhcpcd >> " << output;

    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "DHCPCD has been disabled.";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "DHCPCD disable failed.";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool dhcpcd_disable()
{
    QProcess process;
    process.start("sudo systemctl disable dhcpcd");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    qDebug() << "sudo systemctl disable dhcpcd >> " << output;


    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "DHCPCD has been disabled.";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "DHCPCD disable failed.";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool NetworkManagerEnable(){
    QProcess process;
    process.start("sudo systemctl enable NetworkManager");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    qDebug() << "sudo systemctl enable NetworkManager >> " << output;

    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "sudo systemctl enable NetworkManager OK";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "sudo systemctl enable NetworkManager FAIL";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool NetworkManagerRestart(){
    QProcess process;
    process.start("sudo systemctl restart NetworkManager");
    process.waitForFinished();
    //QString output = process.readAllStandardOutput();
    //qDebug() << "sudo systemctl enable NetworkManager >> " << output;

    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "sudo systemctl restart NetworkManager OK";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "sudo systemctl restart NetworkManager FAIL";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool nmcliRescanWifi(){
    QProcess process;
    process.start("nmcli device wifi rescan");
    process.waitForFinished();
    //QString output = process.readAllStandardOutput();
    //qDebug() << "sudo systemctl enable NetworkManager >> " << output;

    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "nmcli device wifi rescan OK";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "nmcli device wifi rescan FAIL";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool NetworkManagerStart(){
    QProcess process;
    process.start("sudo systemctl start NetworkManager"); //tanpa respon
    process.waitForFinished();
    //QString output = process.readAllStandardOutput();

    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "sudo systemctl start NetworkManager OK";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "sudo systemctl start NetworkManager FAIL";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool nmcliSleep(){
    QProcess process;
    process.start("sleep 5"); //tanpa respon
    process.waitForFinished();
    //QString output = process.readAllStandardOutput();

    if (process.exitStatus() == QProcess::NormalExit) {
        //label->setText("WiFi (wlan0) disabled successfully.");
        qDebug() << "sleep 5 OK";
        return  true;
    } else {
       //label->setText("Failed to disable WiFi (wlan0).");
       qDebug() << "sleep 5 FAIL";
       return false;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QStringList nmcliGetWifiList(){
    QProcess process;

    process.start(QStringLiteral("nmcli"),
                  QStringList() << "-t" << "-f" << "SSID"
                                << "device" << "wifi" << "list");

    process.waitForFinished();

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();

    QStringList ssidList = output.split('\n', Qt::SkipEmptyParts);

    ssidList.removeAll("");   // buang SSID kosong

    return ssidList;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool nmcliConnectToWiFi(const QString &ssid, const QString &password) {
    QProcess process;

    // Format perintah nmcli
    //QString command = QString("nmcli device wifi connect \"%1\" password \"%2\"")
    //                      .arg(ssid, password);

    QString command = "nmcli device wifi connect \"" + ssid + "\" password  \"" + password + "\"";

    // Jalankan perintah di bash
   // process.start("bash", QStringList() << "-c" << command);
    process.start(command); //("nmcli device wifi connect " + ssid + " password " + password);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    qDebug() << "Command Output:" << output;

    // Periksa apakah ada string "successfully" dalam output
    if (output.contains("successfully", Qt::CaseInsensitive)) {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QString nmcliGetSSID()
{
    QProcess process;

    process.start(QStringLiteral("nmcli"),
                  QStringList() << "-t"
                                << "-f" << "NAME,TYPE"
                                << "connection" << "show" << "--active");

    process.waitForFinished();

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines)
    {
        QStringList parts = line.split(':');
        if (parts.size() == 2 && parts[1] == "wifi")
            return parts[0];
    }

    return QString();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool nmcliInit(){

    if(!dhcpcd_stop()){
        return false;
    }

    if(!dhcpcd_disable()){
       return false;
    }

    if(!NetworkManagerEnable()){
        return false;
    }

    if(!NetworkManagerRestart()){
       return false;
    }

    if(!nmcliRescanWifi()){
      return false;
    }
    nmcliSleep();
    //QThread::sleep(5);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool pingGoogle()
{
    QProcess process;
    QString command = "ping -c 5 8.8.8.8";

    // Start ping command and wait for completion
    process.start("bash", QStringList() << "-c" << command);
    process.waitForFinished();

    // Get the output
    QString output = process.readAllStandardOutput();
    qDebug() << "Ping output:\n" << output;

    // Check for successful responses
    if (output.contains("bytes from")) {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nmcliWifiOff()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "nmcli radio wifi off");
    process.waitForFinished();
    qDebug() << "WiFi disabled: " << process.readAllStandardOutput();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void nmcliWifiOn()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "nmcli radio wifi on");
    process.waitForFinished();
    qDebug() << "WiFi disabled: " << process.readAllStandardOutput();
}
