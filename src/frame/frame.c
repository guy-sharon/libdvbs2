#include "frame.h"
#include <string.h>

#define MATYPE_OFFSET_BITS                  ( 0 )


void frame_init(frame_t *frame) {
    frame->matype_ptr = frame->buffer + MATYPE_OFFSET_BITS;
}

void frame_set_matype(frame_t *frame, uint8_t matype[2]) {
    memcpy(frame->matype_ptr, matype, 2);
}

void frame_get_matype(frame_t *frame, matype_t *matype) {
    matype_from_bytes(frame->matype_ptr, matype);
}