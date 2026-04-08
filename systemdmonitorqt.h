#ifndef SYSTEMDMONITORQT_H
#define SYSTEMDMONITORQT_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QProcess>
#include <QRegularExpression>
#include <QDBusReply>

class systemdmonitorqt : public QObject
{
    Q_OBJECT
public:
    explicit systemdmonitorqt(const QString& service, QObject *parent);
        //: QObject(parent), serviceName(service);

    //explicit SystemdMonitorQt(const QString& service, QObject *parent = nullptr);

signals:
    void serviceStarted();
    void serviceStopped();
    void serviceFailed();

private slots:
    void onPropertiesChanged(QString interface,
                             QVariantMap changed,
                             QStringList invalidated);

private:
    QString serviceName;
    QString unitPath;

    void connectToService();
    QString getUnitPath(const QString &service);
    static QString encodeService(const QString &service);
};

#endif // SYSTEMDMONITORQT_H
