#pragma once

#include <QQueue>
#include <QtSerialBus/QCanBusDevice>
#include <QtSerialPort/QSerialPort>

class CLX000CanBus : public QCanBusDevice {
public:
    static QList<QCanBusDeviceInfo> interfaces();
protected:
    QQueue<char> queue;
    QVector<QCanBusFrame> frames;
    void attemptParse();
    bool attemptParseSequence(QByteArray sequence);
    bool packFrame(QCanBusFrame const& frame, QByteArray &packedFrame);
    quint64 frameCount;
};
