#include "gpio.h"

gpio::gpio(QObject *parent)
    : QObject{parent}
{}

//------------------------------------------------------------------------------------------------------------------------
int gpio::setupGPIO()
{
#ifdef PLATFORM_LINUX
    const char *chipname = "gpiochip4";   // RPi 5
    const unsigned int GPIO17 = 17;
    const unsigned int GPIO27 = 27;
    const unsigned int GPIO22 = 22;

    gpiod_chip *chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        qCritical() << "Failed to open" << chipname;
        return -1;
    }

    line17 = gpiod_chip_get_line(chip, GPIO17);
    line27 = gpiod_chip_get_line(chip, GPIO27);
    line22 = gpiod_chip_get_line(chip, GPIO22);

    if (!line17 || !line27) {
        qCritical() << "Failed to get GPIO lines";
        gpiod_chip_close(chip);
        return -1;
    }

    if (gpiod_line_request_output(line17, "qt-gpio17", 0) < 0 ||
        gpiod_line_request_output(line27, "qt-gpio27", 0) < 0 ||
        gpiod_line_request_output(line22, "qt-gpio22", 0) < 0) {

        qCritical() << "Failed to request GPIO output";
        gpiod_chip_close(chip);
        return -1;
    }

    qDebug() << "GPIO 17 & 27 r& 22 eady.";
#endif

    return 1;
}

//------------------------------------------------------------------------------------------------------------------------
void gpio::setColor(qint8 color)
{
#ifdef PLATFORM_LINUX
    int val17 = 0;
    int val27 = 0;
    int val22 = 0;

    switch (color) {
    case 0: val17 = 0; val27 = 0; break; // 00
    case 1: val17 = 0; val27 = 1; break; // 01
    case 2: val17 = 1; val27 = 0; break; // 10
    case 3: val17 = 1; val27 = 1; break; // 11
    default: val17 = 0; val27 = 0; break; // 00
    }

    gpiod_line_set_value(line17, val17);
    gpiod_line_set_value(line27, val27);
    gpiod_line_set_value(line22, val22);


    qDebug() << "color " << color << " GPIO17 =" << val17 << "GPIO27 =" << val27 << "GPIO22 =" << val22;
#endif
}
