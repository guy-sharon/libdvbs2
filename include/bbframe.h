#ifndef DVBS2_BBFRAME_H
#define DVBS2_BBFRAME_H

#include "stdint.h"
#include "stddef.h"
#include "bbheader.h"

#define PBRS_INITIAL_SEQUENCE       0x4A80

void bbframe_init_scramble();
uint8_t bbframe_get_next_scramble_byte();

void bbframe_scramble(uint8_t *data, size_t length);
void bbframe_descramble(uint8_t *data, size_t length);

#endif /* DVBS2_BBFRAME_H */