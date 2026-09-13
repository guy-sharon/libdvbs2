#include "frame.h"
#include <string.h>

#define MATYPE_OFFSET_BITS                  ( 0 )

void frame_init(frame_t *framePtr, modcod_t modcod) {
    framePtr->matype_ptr = framePtr->buffer + MATYPE_OFFSET_BITS;
    memcpy(&framePtr->modcod, &modcod, sizeof(modcod));
}

void frame_set_matype(frame_t *framePtr, uint8_t matype[2]) {
    memcpy(framePtr->matype_ptr, matype, 2);
}

void frame_get_matype(frame_t *framePtr, matype_t *matype) {
    matype_from_bytes(framePtr->matype_ptr, matype);
}

void frame_scramble(frame_t *framePtr) {
    bbframe_scramble(framePtr->buffer, framePtr->modcod.kbch);
}