#include "CLX000CanBusUSB.h"

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

CLX000CanBusUSB::CLX000CanBusUSB(QString interface) : port(interface) {
    port.setReadBufferSize(1024*1024*10);

    connect(
        &port,
        SIGNAL(readyRead()),
        this,
        SLOT(dataAvailable())
    );

    queue.reserve(1024*1024);
}

bool CLX000CanBusUSB::open() {
    qDebug() << "CLX000CanBusUSB - open";
    frameCount = 0;
    setState(CanBusDeviceState::ConnectingState);

    bool status = port.open(QSerialPort::OpenModeFlag::ReadWrite);

    if(status) {
        setState(CanBusDeviceState::ConnectedState);
    }

    return status;
}

void CLX000CanBusUSB::close() {
    setState(CanBusDeviceState::ClosingState);

    qDebug() << "CLX000CanBusUSB - close";

    setState(CanBusDeviceState::UnconnectedState);

    port.close();
}

bool CLX000CanBusUSB::writeFrame(const QCanBusFrame &frame) {
    bool resultFlag = false;

    QByteArray packedFrame;
    resultFlag = packFrame(frame, packedFrame);
    qint64 bytesWritten = 0;

    if( resultFlag ) {
        bytesWritten = port.write(packedFrame);

        resultFlag = (bytesWritten == packedFrame.length());
    }

    return resultFlag;
}

QString CLX000CanBusUSB::interpretErrorFrame(const QCanBusFrame &errorFrame) {
    return QString();
}

void CLX000CanBusUSB::dataAvailable() {
    auto const data = port.read(1024);

    // Copy all data except the zero termination byte.
    std::copy_n(data.cbegin(), data.length(), std::back_inserter(queue));

    attemptParse();
}
