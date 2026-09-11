#ifndef DVBS2_FRAME_H
#define DVBS2_FRAME_H

#include "stdint.h"
#include "matype.h"

#define MAX_FRAME_SIZE_BITS     64800

typedef struct {
    uint8_t buffer[MAX_FRAME_SIZE_BITS];

    uint8_t *matype_ptr;
} frame_t;

void frame_init(frame_t *frame);

void frame_set_matype(frame_t *frame, uint8_t matype[2]);
void frame_get_matype(frame_t *frame, matype_t *matype);

#endif /* DVBS2_FRAME_H */