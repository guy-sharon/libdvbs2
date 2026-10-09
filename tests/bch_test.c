#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"

#include "bch.h"
#include "modcod.h"

static void test_encode_decode(modcod_t modcod, size_t num_errors) {
    bch_init(modcod);

    uint8_t *frame = calloc(modcod.nbch, sizeof(*frame));
    uint8_t *expected = calloc(modcod.nbch, sizeof(*expected));
    assert(frame != NULL && expected != NULL);

    for (size_t i = 0; i < modcod.kbch; i++) {
        frame[i] = (uint8_t)((i * 37u + 11u) & 1u);
    }

    bch_encode(frame);
    memcpy(expected, frame, modcod.nbch);

    for (size_t i = 0; i < num_errors; i++) {
        const size_t pos = ((i + 1u) * 37u * (i + 2u)) % modcod.nbch;
        frame[pos] ^= 1u;
    }

    assert(bch_decode(frame));
    assert(memcmp(frame, expected, modcod.nbch) == 0);

    free(frame);
    free(expected);
}

int main(void)
{
    test_encode_decode(SHORT_1_4, 3);
    return 0;
}