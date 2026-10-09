#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bch.h"
#include "modcod.h"

static void test_encode_decode_corrects_three_errors(void)
{
    enum { MSG_LEN = 64 };
    bch_init(SHORT_1_4);

    const size_t parity_len = bch_parity_bytes();
    const size_t codeword_len = MSG_LEN + parity_len;
    uint8_t msg[MSG_LEN];
    uint8_t decoded[MSG_LEN];
    uint8_t *codeword = calloc(codeword_len, sizeof(*codeword));
    uint8_t *expected = calloc(codeword_len, sizeof(*expected));

    assert(codeword != NULL && expected != NULL);
    for (size_t i = 0; i < MSG_LEN; i++) {
        msg[i] = (uint8_t)((i * 37 + 11) & 1);
    }

    bch_encode(msg, MSG_LEN, codeword);
    memcpy(codeword + parity_len, msg, MSG_LEN);
    memcpy(expected, codeword, codeword_len);
    codeword[0] ^= 1;
    codeword[37] ^= 1;
    codeword[parity_len + 12] ^= 1;

    assert(bch_decode(codeword, codeword_len, decoded));
    assert(memcmp(decoded, msg, MSG_LEN) == 0);
    assert(memcmp(codeword, expected, codeword_len) == 0);
    free(codeword);
    free(expected);
}

int main(void)
{
    test_encode_decode_corrects_three_errors();
    return 0;
}