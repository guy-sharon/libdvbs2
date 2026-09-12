#include "bbframe.h"
#include "bbheader.h"

static uint16_t lfsr = PBRS_INITIAL_SEQUENCE;

void bbframe_init_scramble(void) {
    lfsr = PBRS_INITIAL_SEQUENCE;
}

uint8_t bbframe_get_next_scramble_byte(void) {
    uint8_t scramble_byte = 0;
    for (int bit = 0; bit < 8; bit++) {
        uint8_t feedback = ((lfsr) ^ (lfsr >> 1)) & 1;
        lfsr >>= 1;
        lfsr |= (feedback << 14);
        scramble_byte |= (feedback << bit);
    }
    return scramble_byte;
}

void bbframe_scramble(uint8_t *data, size_t length) {
    bbframe_init_scramble();

    for (size_t i = 0; i < length; i++) {
        data[i] ^= bbframe_get_next_scramble_byte();
    }
}

void bbframe_descramble(uint8_t *data, size_t length) {
    bbframe_scramble(data, length);
}