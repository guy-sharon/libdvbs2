#include "bbheader.h"
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

void bbheader_calc_crc(bbheader_t *header) {
    header->crc = crc8((uint8_t *)header, sizeof(bbheader_t) - 1);
}