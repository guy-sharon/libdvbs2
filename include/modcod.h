#ifndef DVBS2_MODCOD_H
#define DVBS2_MODCOD_H

#include "stdint.h"
#include "stdbool.h"

/*
 * FEC coding parameters from EN 302 307-1, Tables 5a / 5b.
 *
 * The DVB-S2 MODCOD field (5 bits in the PL header) selects a
 * (modulation, code-rate) pair.  Code rate determines these
 * parameters; the modulation adds no extra coding parameters.
 *
 * Modulation availability by rate (clause 4.3):
 *   QPSK:    1/4  1/3  2/5  1/2  3/5  2/3  3/4  4/5  5/6  8/9  9/10
 *   8PSK:                  --   --  3/5  2/3  3/4  4/5  5/6  8/9  9/10
 *   16APSK:                 --   --  --    2/3  3/4  4/5  5/6  8/9  9/10
 *   32APSK:                 --    --   --   --   3/4  4/5  5/6  8/9  9/10
 */

typedef struct {
    bool short_frame; /* true for short FECFRAME, false for normal FECFRAME */
    uint16_t kbch;
    uint16_t nbch;
    uint16_t kldpc;
    uint16_t nldpc;
    uint8_t bch_t;
} modcod_t;

#define MODCOD(name, _short_frame, _kbch, _kldpc, _bch_t, _nldpc) \
    static const modcod_t name = {.short_frame=_short_frame, \
                                  .kbch=_kbch, .nbch=_kldpc, .kldpc=_kldpc, \
                                  .bch_t=_bch_t, \
                                  .nldpc=_nldpc}

#define MODCOD_SHORT(prefix, _kbch, _kldpc, _bch_t) \
    MODCOD(SHORT_##prefix, true, _kbch, _kldpc, _bch_t, 16200)

#define MODCOD_NORMAL(prefix, _kbch, _kldpc, _bch_t) \
    MODCOD(NORMAL_##prefix, false, _kbch, _kldpc, _bch_t, 64800)

/* Normal FECFRAME — nldpc = 64 800  (Table 5a) */

MODCOD_NORMAL(1_4,  16008, 16200, 12); /* QPSK        */
MODCOD_NORMAL(1_3,  21408, 21600, 12); /* QPSK        */
MODCOD_NORMAL(2_5,  25728, 25920, 12); /* QPSK        */
MODCOD_NORMAL(1_2,  32208, 32400, 12); /* QPSK        */
MODCOD_NORMAL(3_5,  38688, 38880, 12); /* QPSK, 8PSK  */
MODCOD_NORMAL(2_3,  43040, 43200, 10); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_NORMAL(3_4,  48408, 48600, 12); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_NORMAL(4_5,  51648, 51840, 12); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_NORMAL(5_6,  53840, 54000, 10); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_NORMAL(8_9,  57472, 57600,  8); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_NORMAL(9_10, 58192, 58320,  8); /* QPSK, 8PSK, 16APSK, 32APSK */

/* Short FECFRAME — nldpc = 16 200  (Table 5b) */

MODCOD_SHORT(1_4,  3072,  3240,  12); /* QPSK        */
MODCOD_SHORT(1_3,  5232,  5400,  12); /* QPSK        */
MODCOD_SHORT(2_5,  6312,  6480,  12); /* QPSK        */
MODCOD_SHORT(1_2,  7032,  7200,  12); /* QPSK        */
MODCOD_SHORT(3_5,  9552,  9720,  12); /* QPSK,	8PSK	*/
MODCOD_SHORT(2_3, 10632, 10800,	 12); /* QPSK,	8PSK,	16APSK,	32APSK */
MODCOD_SHORT(3_4, 11712, 11880,  12); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_SHORT(4_5, 12432, 12600,  12); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_SHORT(5_6, 13152, 13320,  12); /* QPSK, 8PSK, 16APSK, 32APSK */
MODCOD_SHORT(8_9, 14232, 14400,  12); /* QPSK, 8PSK, 16APSK, 32APSK */

#endif /* DVBS2_MODCOD_H */