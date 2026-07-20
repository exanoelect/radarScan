/*#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
*/



#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QSystemTrayIcon>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("radarScan");

    // Aplikasi tetap hidup walaupun tidak ada window yang tampil.
    app.setQuitOnLastWindowClosed(false);

    // Tetap dibuat jika proses utama aplikasi berada di MainWindow.
    MainWindow window;

    // Jangan pernah tampilkan GUI.
    window.hide();

    QIcon icon("/home/pi/app/logos.png");

    if (icon.isNull()) {
        icon = window.windowIcon();
    }

    if (icon.isNull()) {
        icon = QIcon::fromTheme("network-wireless");
    }

    QSystemTrayIcon trayIcon(icon);
    trayIcon.setToolTip("radarScan aktif");

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "System tray tidak tersedia";
    }

    // Hanya tampilkan ikon.
    trayIcon.show();

    // Tidak ada setContextMenu().
    // Tidak ada connect activated().
    // Klik ikon tidak melakukan apa pun.

    return app.exec();
}

