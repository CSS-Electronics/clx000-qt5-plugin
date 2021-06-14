#include "CLX000CanBus.h"

#include <QDataStream>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>

#include "CRC16.h"

QList<QCanBusDeviceInfo> CLX000CanBus::interfaces() {
    QList<QCanBusDeviceInfo> result;

    static QString const description("CLX000");

    // Find all available USB-CDC devices.
    auto allAvailablePorts = QSerialPortInfo::availablePorts();

    for(auto const& portInfo: allAvailablePorts) {
        // Remove all non-CDC ports.
        if(!portInfo.hasVendorIdentifier() || !portInfo.hasProductIdentifier()) {
            continue;
        }

        // Take only devices matching the USB identifier of the CLX000 CDC mode.
        if( (portInfo.vendorIdentifier() != quint16(0x1CBEu)) ||
            (portInfo.productIdentifier() != quint16(0x021Au)) ) {
            continue;
        }

        qDebug() << "CLX000CanBus - found device" << portInfo.portName() << portInfo.serialNumber();

        // Create new interface description for this.
        result.push_back(
            createDeviceInfo(
                portInfo.portName(),
                portInfo.serialNumber(),
                description,
                0,
                false,
                false
            )
        );
    }

    return result;
}

void CLX000CanBus::attemptParse() {
    // Check if enough data is in the queue to contain a frame. Minimal length is 16.
    if(queue.length() < 16){
        return;
    }

    while(queue.length() >= 16) {
        // Parse in loop.

        // Look for a sequence starting and ending with 0x7e.
        auto first = queue.indexOf(0x7e);

        if(first < 0) {
            // No more entries matching.
            break;
        }

        auto last = queue.indexOf(0x7e, first + 1);

        if(last < 0) {
            // No more entries matching.
            break;
        }

        // The end is inclusive.
        auto const length = last - first + 1;

        // Extract the sequence.
        QByteArray data;
        data.reserve(length);

        for(auto i = 0; i < length; ++i) {
            auto val = queue.dequeue();
            data.push_back(val);
        }

        if(!attemptParseSequence(data)) {
            qDebug() << "Parse error after" << frameCount;
        }
    }

    if(!frames.isEmpty()) {
        // Pass all frames to the super class, and reset for next parse pass.
        enqueueReceivedFrames(frames);
        frameCount += frames.length();
        frames.clear();
    }

}

bool CLX000CanBus::attemptParseSequence(QByteArray sequence) {
    QCanBusFrame frame;
    QByteArray destuffed;

    // Create sub-sequence without the markers for frame beginning and end.
    QByteArray subSequence = sequence.mid(1, sequence.length() - 2);

    // Remove any stuffing bytes.
    auto iter = subSequence.begin();

    while(iter != subSequence.end()) {
        auto current = *iter++;

        // Check for stuffing byte.
        if(current == 0x7d) {
            current = *iter++;
            current ^= 0b00100000;
        }

        destuffed.push_back(current);
    }

    // Extract checksum.
    auto checksumData = destuffed.right(2);
    destuffed.chop(2);
    QDataStream dataStream2(checksumData);
    dataStream2.setByteOrder(QDataStream::ByteOrder::BigEndian);
    uint16_t cc;
    dataStream2 >> cc;

    uint16_t const checksum = calculateCRC16(destuffed);

    if(checksum != cc) {
        qDebug() << "Checksum error, calculated" << checksum << "expected" << cc << "data" << destuffed.toHex();
        qDebug() << sequence.toHex();
        qDebug() << subSequence.toHex();
        return false;
    }

    QDataStream dataStream(destuffed);
    dataStream.setByteOrder(QDataStream::ByteOrder::BigEndian);

    // Direction.
    quint8 direction;
    dataStream >> direction;

    frame.setLocalEcho(direction == 0x02u);

    // Decode time.
    quint32 seconds;
    dataStream >> seconds;

    quint16 milliSeconds;
    dataStream >> milliSeconds;

    auto ts = QCanBusFrame::TimeStamp(quint64(seconds), quint64(milliSeconds) * 1000);

    frame.setTimeStamp(ts);

    // Decode ID.
    quint32 id;
    dataStream >> id;

    frame.setExtendedFrameFormat(id > 0x7FFu);
    frame.setFrameId(id & 0x1FFFFFFF);

    // Decode DLC.
    quint8 dlc;
    dataStream >> dlc;

    // Extract data.
    frame.setPayload(destuffed.right(dlc));

    frames.push_back(frame);

    return true;
}

bool CLX000CanBus::packFrame(QCanBusFrame const& frame, QByteArray &packedFrame) {
    QByteArray payloadArray;
    QDataStream payloadStream(&payloadArray, QIODevice::ReadWrite);
    payloadStream.setByteOrder(QDataStream::ByteOrder::BigEndian);

    payloadStream << static_cast<quint8>(0x03);

    /* Write CAN ID (Big endian) */
    quint32 canID = frame.frameId() & 0x1FFFFFFF;
    if( frame.hasExtendedFrameFormat() ) {
        canID |= 0x20000000;
    }
    payloadStream << canID;

    int dataLength = frame.payload().length();
    if(0 <= dataLength && dataLength <= 8) {
        auto DLC = static_cast<quint8>(dataLength);
        payloadStream << DLC;
    }

    payloadStream.writeRawData(frame.payload().data(), dataLength);

    /* Calculate checksum */
    quint16 checksum = calculateCRC16(payloadArray);

    /* Write checksum */
    payloadStream << checksum;

    /* Escape any 0x7E sequences */
    packedFrame.append(0x07E);
    for (char const& byteValue: payloadArray) {
        if( byteValue == 0x7E ) {
            packedFrame.append(static_cast<quint8>(0x7D));
            packedFrame.append(static_cast<quint8>(byteValue ^ 0b00100000) );
        } else {
            packedFrame.append(byteValue );
        }
    }
    packedFrame.append(0x07E);

    return true;
}
