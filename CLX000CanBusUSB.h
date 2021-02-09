#pragma once

#include <QtSerialBus/QCanBusDevice>
#include <QtSerialPort/QSerialPort>

#include "CLX000CanBus.h"

class CLX000CanBusUSB : public CLX000CanBus {
Q_OBJECT
public:
    explicit CLX000CanBusUSB(QString interface);

    bool open() override;
    void close() override;
    bool writeFrame(const QCanBusFrame &frame) override;
    QString interpretErrorFrame(const QCanBusFrame &errorFrame) override;
private:
    QSerialPort port;
private slots:
    void dataAvailable();
};
