#include "crc.h"

uint8_t crc8(const uint8_t *data, size_t length) {
    uint8_t crc = 0x00;
    const uint8_t polynomial = CRC8_POLYNOMIAL;

    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}