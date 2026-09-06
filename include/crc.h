#ifndef DVBS2_CRC_H
#define DVBS2_CRC_H

#include <stdint.h>
#include <stddef.h>

// Reversed polynomial for X8+X7+X6+X4+X2+1 (page 16)
#define CRC8_POLYNOMIAL     0xAB

uint8_t crc8(const uint8_t *data, size_t length);

#endif