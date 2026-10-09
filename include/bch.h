#ifndef DVBS2_BCH_H
#define DVBS2_BCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "modcod.h"

void bch_init(modcod_t modcod);
void bch_encode(uint8_t *frame);
bool bch_decode(uint8_t *frame);

#endif /* DVBS2_BCH_H */