#ifndef GPIO_H
#define GPIO_H

#include <QObject>

class gpio : public QObject
{
    Q_OBJECT
public:
    explicit gpio(QObject *parent = nullptr);

signals:
public slots:
    //GPIO LEd strip
    int setupGPIO();
    void setColor(qint8 color);
};

#endif // GPIO_H
