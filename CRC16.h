#pragma once

#include <cstdint>

#include <QByteArray>

enum struct CRC16Type {
    CRC_16_IBM
};

uint16_t calculateCRC16(QByteArray data, CRC16Type crc16Type = CRC16Type::CRC_16_IBM);
