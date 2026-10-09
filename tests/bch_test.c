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

    // Inject errors
    for (size_t i = 0; i < num_errors; i++) {
        const size_t pos = ((i + 1u) * 37u * (i + 2u)) % modcod.nbch;
        frame[pos] ^= 1u;
    }

    bool should_work = num_errors <= modcod.bch_t;
    assert(bch_decode(frame) == should_work);
    if (should_work) {
        assert(memcmp(frame, expected, modcod.nbch) == 0);
    }

    free(frame);
    free(expected);
}

int main(void)
{
    const modcod_t modcods[] = {
        SHORT_1_4, SHORT_1_3, SHORT_2_5, SHORT_1_2, SHORT_3_5,
        SHORT_2_3, SHORT_3_4, SHORT_4_5, SHORT_5_6, SHORT_8_9,
        NORMAL_1_4, NORMAL_1_3, NORMAL_2_5, NORMAL_1_2, NORMAL_3_5,
        NORMAL_2_3, NORMAL_3_4, NORMAL_4_5, NORMAL_5_6, NORMAL_8_9,
        NORMAL_9_10
    };

    for (size_t i = 0; i < sizeof(modcods) / sizeof(modcods[0]); i++) {
        for (int num_errors = 0; num_errors <= 2*modcods[i].bch_t + 1; num_errors++) {
            test_encode_decode(modcods[i], num_errors);
        }
    }

    return 0;
}