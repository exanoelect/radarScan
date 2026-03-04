#ifndef GPIO_H
#define GPIO_H

#include <QObject>
#include <qdebug.h>

#ifdef PLATFORM_LINUX

extern "C" {
#include <gpiod.h>
}
#endif

class gpio : public QObject
{
    Q_OBJECT
public:
    explicit gpio(QObject *parent = nullptr);
    ~gpio();

signals:
public slots:
    //GPIO LEd strip
    int setupGPIO();
    void setColor(qint8 color);

private:
#ifdef PLATFORM_LINUX
    gpiod_line *line17;
    gpiod_line *line27;
    gpiod_line *line22;
#endif
};

#endif // GPIO_H
