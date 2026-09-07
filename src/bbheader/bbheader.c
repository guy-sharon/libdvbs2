#include "bbheader/bbheader.h"
#include "bbheader/matype.h"
#include "crc.h"

#include <stdint.h>

 /*
 * Packetised Stream:
 *
 *  ----------|----|-------------|----|------------|----|------------|
 *     UP     |CRC8|     UP      |CRC8|     UP     |CRC8|     UP     |
 *  ----------|----|-------------|----|------------|----|------------|
 *                    ^          ^                                |
 *                    |<--SYNCD-->                                |
 *                    |                                           |
 * BBFRAME:           |                                           |
 *  |<--- 80 bits --->|<------------------ DFL ------------------>|
 *  +-----------------+-------------------------------------------+
 *  |    BBHEADER     |               DATA FIELD                  |
 *  +-----------------+-------------------------------------------+
 *
 * SYNCD = distance (in bits) from the start of the Data Field to the
 *         start of the first User Packet (UP) that begins within it.
 */

typedef struct {
    matype_t matype;
    uint16_t upl; // User Packet Length in bits (188x8 for transport stream)
    uint16_t dfl; // Data Field Length in bits, in the range 0 to 58 112.
    uint8_t sync; // copy of the User Packet Sync-byte (0x47 for transport stream)
    uint8_t syncd[2]; // distance in bits from the beginning of the DATA FIELD and the
                      // first UP from this frame (first bit of the CRC-8).
    uint8_t crc;
} bbheader_t;

const int bbheader_size = sizeof(bbheader_t);

void bbheader_calc_crc(bbheader_t *header) {
    header->crc = crc8((uint8_t *)header, sizeof(bbheader_t) - 1);
}