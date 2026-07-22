#ifndef GPIO_H
#define GPIO_H

#include <QObject>
#include <QDebug>

#ifdef Q_OS_LINUX
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

public slots:
    int setupGPIO();
    void setColor(qint8 color);
    void setPin(quint8 pin, quint8 level);

private:
#ifdef Q_OS_LINUX
    gpiod_chip *m_chip = nullptr;

    gpiod_line *line17 = nullptr;
    gpiod_line *line27 = nullptr;
    gpiod_line *line22 = nullptr;
#endif
};

#endif // GPIO_H
