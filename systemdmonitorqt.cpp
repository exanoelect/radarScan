#include "systemdmonitorqt.h"
#include <QDBusMessage>
#include <QDebug>

systemdmonitorqt::systemdmonitorqt(const QString& service, QObject *parent)
    : QObject(parent), serviceName(service)
{
    //QString encoded = encodeService(service);

    //unitPath = "/org/freedesktop/systemd1/unit/" + encoded;
    unitPath = getUnitPath(service);
    if(unitPath.isEmpty()){
        qWarning() << "Invalid path ";
        return;
    }

    connectToService();
}

//--------------------------------------------------------------
void systemdmonitorqt::onPropertiesChanged(QString interface, QVariantMap changed, QStringList invalidated)
{
    if (interface != "org.freedesktop.systemd1.Unit")
        return;

    if (changed.contains("ActiveState")){
        QString state = changed["ActiveState"].toString();

        qDebug() << "Service state changed:" << state;

        if (state == "active")
            emit serviceStarted();
        else if (state == "inactive")
            emit serviceStopped();
        else if (state == "failed")
            emit serviceFailed();
    }

    if (changed.contains("SubState")){
        QString sub = changed["SubState"].toString();
        if (sub == "running") emit serviceStarted();
    }
}

//--------------------------------------------------------------
void systemdmonitorqt::connectToService()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    bool ok = bus.connect(
        "org.freedesktop.systemd1",
        unitPath,
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(onPropertiesChanged(QString,QVariantMap,QStringList))
    );

    if (!ok)
    {
        qWarning() << "Failed to connect DBus signal";
    }

}

//--------------------------------------------------------------
QString systemdmonitorqt::getUnitPath(const QString &service)
{
    QDBusInterface iface(
         "org.freedesktop.systemd1",
         "/org/freedesktop/systemd1",
         "org.freedesktop.systemd1.Manager",
         QDBusConnection::systemBus()
    );

    QDBusReply<QDBusObjectPath> reply =
        iface.call("GetUnit", service);

    if(!reply.isValid()) {
        qWarning() << "DBus GetUnit failed:" << reply.error().message();
        return "";
    }

    return reply.value().path();
}

//--------------------------------------------------------------
QString systemdmonitorqt::encodeService(const QString &service)
{

    QString encoded;
#ifdef Q_OS_LINUX
    for (QChar c : service)
    {
        if (c.isLetterOrNumber())
            encoded += c;
        else
            encoded += QString("_%1").arg(c.unicode(), 2, 16, QChar('0'));
    }
#endif
    return encoded;
}
