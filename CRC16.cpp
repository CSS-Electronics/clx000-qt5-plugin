#include <QBitArray>

#include "CRC16.h"

uint16_t calculateCRC16(QByteArray data, CRC16Type crc16Type) {
    uint16_t const initial = 0x0000u;
    uint16_t const polynomial = 0xA001;
    uint16_t const final = 0x0000;

    uint16_t crc = initial;

    for(char const& value: data) {
        // Reinterpret from signed to unsigned.
        uint8_t d = value;

        crc ^= d;

        for(auto i = 0; i < 8; ++i) {
            if (crc & 0x0001u) {
                crc >>= 1;
                crc ^= polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc ^ final;
}
