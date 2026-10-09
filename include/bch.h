#ifndef DVBS2_BCH_H
#define DVBS2_BCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "modcod.h"

void bch_init(modcod_t modcod);
size_t bch_parity_bytes(void);
void bch_encode(uint8_t *msg, size_t msg_len, uint8_t *bchfec);
bool bch_decode(uint8_t *codeword, size_t codeword_len, uint8_t *msg);

#endif /* DVBS2_BCH_H */