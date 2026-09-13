#ifndef DVBS2_FRAME_H
#define DVBS2_FRAME_H

#include "stdint.h"
#include "bbframe.h"
#include "matype.h"
#include "modcod.h"

#define MAX_FRAME_SIZE_BITS     64800

typedef struct {
    uint8_t buffer[MAX_FRAME_SIZE_BITS];

    modcod_t modcod;

    uint8_t *matype_ptr;
    uint8_t *upl_ptr;
    uint8_t *dfl_ptr;
    uint8_t *sync_ptr;
    uint8_t *syncd_ptr;
    uint8_t *crc8_ptr;
} frame_t;

void frame_init(frame_t *framePtr, modcod_t modcod);

void frame_set_matype(frame_t *frame, uint8_t matype[2]);
void frame_get_matype(frame_t *frame, matype_t *matype);
void frame_scramble(frame_t *framePtr);

#endif /* DVBS2_FRAME_H */