#include "gpio.h"

#ifdef Q_OS_LINUX
extern "C" {
#include <gpiod.h>
}
#endif

gpio::gpio(QObject *parent)
    : QObject(parent)
{
}

gpio::~gpio()
{
#ifdef Q_OS_LINUX
    if (line17) {
        gpiod_line_set_value(line17, 0);
        gpiod_line_release(line17);
        line17 = nullptr;
    }

    if (line27) {
        gpiod_line_set_value(line27, 0);
        gpiod_line_release(line27);
        line27 = nullptr;
    }

    if (line22) {
        gpiod_line_set_value(line22, 0);
        gpiod_line_release(line22);
        line22 = nullptr;
    }

    if (m_chip) {
        gpiod_chip_close(m_chip);
        m_chip = nullptr;
    }
#endif
}

// ---------------------------------------------------------------------------

int gpio::setupGPIO()
{
#ifdef Q_OS_LINUX
    const char *chipName = "gpiochip4";

    constexpr unsigned int GPIO17 = 17;
    constexpr unsigned int GPIO27 = 27;
    constexpr unsigned int GPIO22 = 22;

    m_chip = gpiod_chip_open_by_name(chipName);

    if (!m_chip) {
        qCritical() << "Failed to open" << chipName;
        return -1;
    }

    line17 = gpiod_chip_get_line(m_chip, GPIO17);
    line27 = gpiod_chip_get_line(m_chip, GPIO27);
    line22 = gpiod_chip_get_line(m_chip, GPIO22);

    if (!line17 || !line27 || !line22) {
        qCritical() << "Failed to get GPIO lines.";
        return -1;
    }

    if (gpiod_line_request_output(line17, "qt-gpio17", 0) < 0) {
        qCritical() << "Failed to request GPIO17.";
        return -1;
    }

    if (gpiod_line_request_output(line27, "qt-gpio27", 0) < 0) {
        qCritical() << "Failed to request GPIO27.";
        return -1;
    }

    if (gpiod_line_request_output(line22, "qt-gpio22-pwm", 0) < 0) {
        qCritical() << "Failed to request GPIO22.";
        return -1;
    }

    qDebug() << "GPIO17, GPIO27, and GPIO22 ready.";
#endif

    return 1;
}

// ---------------------------------------------------------------------------

void gpio::setColor(qint8 color)
{
#ifdef Q_OS_LINUX
    int val17 = 0;
    int val27 = 0;

    switch (color) {
    case 0:
        val17 = 0;
        val27 = 0;
        break;

    case 1:
        val17 = 0;
        val27 = 1;
        break;

    case 2:
        val17 = 1;
        val27 = 0;
        break;

    case 3:
        val17 = 1;
        val27 = 1;
        break;

    default:
        val17 = 0;
        val27 = 0;
        break;
    }

    if (line17) {
        gpiod_line_set_value(line17, val17);
    }

    if (line27) {
        gpiod_line_set_value(line27, val27);
    }

    qDebug() << "Color:" << color
             << "GPIO17:" << val17
             << "GPIO27:" << val27;
#else
    Q_UNUSED(color);
#endif
}

// ---------------------------------------------------------------------------

void gpio::setPin(quint8 pin, quint8 level)
{
#ifdef Q_OS_LINUX
    const int gpioLevel = level ? 1 : 0;

    switch (pin) {
    case 0:
        if (line17) {
            gpiod_line_set_value(line17, gpioLevel);
        }
        break;

    case 1:
        if (line27) {
            gpiod_line_set_value(line27, gpioLevel);
        }
        break;

    case 2:
        if (line22) {
            gpiod_line_set_value(line22, gpioLevel);
        }
        break;

    default:
        qWarning() << "Unknown GPIO pin index:" << pin;
        break;
    }
#else
    Q_UNUSED(pin);
    Q_UNUSED(level);
#endif
}
