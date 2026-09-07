#include "bbheader/bbheader.h"
#include "bbheader/matype.h"

#include <stdint.h>

typedef struct {
    matype_t matype;
    uint8_t upl[2];
    uint8_t dfl[2];
    uint8_t sync;
    uint8_t syncd[2];
    uint8_t crc;
} bbheader_t;

const int bbheader_size = sizeof(bbheader_t);