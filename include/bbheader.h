#ifndef DVBS2_BBHEADER_H
#define DVBS2_BBHEADER_H

#include "matype.h"

#define BBHEADER_SIZE_BYTES 10

typedef struct {
    matype_t matype;
    uint16_t upl; // User Packet Length in bits (188x8 for transport stream)
    uint16_t dfl; // Data Field Length in bits, in the range 0 to 58 112.
    uint8_t sync; // copy of the User Packet Sync-byte (0x47 for transport stream)
    uint8_t syncd[2]; // distance in bits from the beginning of the DATA FIELD and the
                      // first UP from this frame (first bit of the CRC-8).
    uint8_t crc;
} bbheader_t;

#endif /* DVBS2_BBHEADER_H */