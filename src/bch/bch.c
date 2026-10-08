#include "bch.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "assert.h"
#include "stdio.h"

// *********************************************************************** //
// ******************************* Statics ******************************* //
// *********************************************************************** //
static uint16_t *alpha_to;
static uint16_t *index_of;
static uint16_t GF_SIZE = 0;
static uint8_t t = 0;

typedef struct {
    uint8_t *coeffs;
    uint16_t deg;
} polynom8_t;

typedef struct {
    uint16_t *coeffs;
    uint16_t deg;
} polynom16_t;

static polynom8_t primitive_poly = {0};
static polynom8_t gen_poly = {0};

#define POLY_FREE(poly) \
    if (poly.coeffs) { \
        free(poly.coeffs); \
    }

#define POLY8_INIT(name, ...) \
    { \
        uint8_t ones[] = __VA_ARGS__; \
        name.deg = ones[0]; \
        name.coeffs = calloc(ones[0]+1, sizeof(uint8_t)); \
        for (long unsigned int i = 0; i < sizeof(ones)/sizeof(ones[0]); i++) { \
            name.coeffs[ones[i]] = 1; \
        } \
    }

#define POLY8_DEFINE(name, ...) \
    polynom8_t name; \
    POLY8_INIT(name, __VA_ARGS__); 

// **************************************************************************** //
// ******************************* Galois Field ******************************* //
// **************************************************************************** //
static uint16_t gf_reduce_exp(int exponent) {
    int reduced = exponent % GF_SIZE;
    return (uint16_t)(reduced < 0 ? reduced + GF_SIZE : reduced);
}

uint16_t gf_mul(uint16_t a, uint16_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }

    return alpha_to[(index_of[a] + index_of[b]) % GF_SIZE];
}

uint16_t gf_div(uint16_t a, uint16_t b) {
    if (a == 0) {
        return 0;
    }

    return alpha_to[gf_reduce_exp(index_of[a] - index_of[b])];
}

uint16_t arr_to_gf(uint8_t *arr, size_t len) {
    uint16_t gf = 0;
    for (size_t i = 0; i < len; i++) {
        gf += arr[i] * (1<<i);
    }
    return gf;
}

void lsfr_step(uint8_t *lsfr, size_t lsfr_len, bool f_in, polynom8_t poly) {
    bool out = lsfr[lsfr_len-1];
    for (int i = lsfr_len-1; i > 0; i--) {
        lsfr[i] = lsfr[i-1] ^ (out & poly.coeffs[i]);
    }
    lsfr[0] = (out & poly.coeffs[0]) ^ f_in;
}

void poly_div(polynom8_t poly_nom, polynom8_t poly_den, polynom8_t *out) {
    const size_t n = poly_nom.deg;
    out->coeffs = calloc(poly_den.deg, sizeof(uint8_t));
    out->deg = poly_den.deg - 1;
    for (size_t i = 0; i < n+1; i++) {
        bool f_in = poly_nom.coeffs[n-i];
        lsfr_step(out->coeffs, poly_den.deg, f_in, poly_den);
    }
}

void poly_mul(polynom8_t poly1, polynom8_t poly2, polynom8_t *out) {
    out->coeffs = calloc(poly1.deg + poly2.deg + 1, sizeof(uint8_t));
    out->deg = poly1.deg + poly2.deg;

    for (int i = 0; i < poly1.deg+1; i++) {
        if (poly1.coeffs[i]) {
            for (int j = 0; j < poly2.deg+1; j++) {
                if (poly2.coeffs[j]) {
                    out->coeffs[i + j] ^= 1;
                }
            }
        }
    }
}

void poly8_add(polynom8_t poly1, polynom8_t poly2, polynom8_t *out) {
    uint16_t deg = poly1.deg > poly2.deg ? poly1.deg : poly2.deg;
    if (out->coeffs == NULL) {
        out->coeffs = calloc(deg+1, sizeof(uint8_t));
    }
    out->deg = deg;
    for (int i = 0; i < deg+1; i++) {
        bool v1 = i > poly1.deg ? 0 : poly1.coeffs[i];
        bool v2 = i > poly2.deg ? 0 : poly2.coeffs[i];
        out->coeffs[i] = v1 ^ v2;
    }
}

void poly16_add(polynom16_t poly1, polynom16_t poly2, polynom16_t *out) {
    uint16_t deg = poly1.deg > poly2.deg ? poly1.deg : poly2.deg;
    if (out->coeffs == NULL) {
        out->coeffs = calloc(deg+1, sizeof(uint16_t));
    }
    out->deg = deg;
    for (int i = 0; i < deg+1; i++) {
        uint16_t v1 = i > poly1.deg ? 0 : poly1.coeffs[i];
        uint16_t v2 = i > poly2.deg ? 0 : poly2.coeffs[i];
        out->coeffs[i] = v1 ^ v2;
    }
}

uint16_t gf_poly_eval_alpha_power(polynom8_t poly, uint16_t power) {
    // only for polys with coeffs 0 and 1
    uint16_t res = 0;
    for (int i = 0; i < poly.deg+1; i++) {
        if (poly.coeffs[i]) {
            res ^= alpha_to[(i*power) % GF_SIZE];
        }
    }
    return res;
}

void build_alpha_table() {
    GF_SIZE = (1<<primitive_poly.deg)-1;
    alpha_to = malloc(GF_SIZE*sizeof(uint16_t));
    index_of = malloc((GF_SIZE+1)*sizeof(uint16_t));

    uint8_t *lsfr = calloc(primitive_poly.deg, sizeof(uint8_t));
    lsfr[0] = 1;
    for (int power = 0; power < GF_SIZE; power++) {
        alpha_to[power] = arr_to_gf(lsfr, primitive_poly.deg);
        index_of[alpha_to[power]] = power;
        lsfr_step(lsfr, primitive_poly.deg, 0, primitive_poly);
    }
    free(lsfr);
}

void calc_syndromes(polynom8_t codeword_poly, uint16_t *syndromes) {
    for (uint16_t power = 1; power < 2*t+1; power++) {
        syndromes[power-1] = gf_poly_eval_alpha_power(codeword_poly, power);
    }
}

void encode(uint8_t *msg, size_t msg_len, uint8_t *bchfec) {
    polynom8_t msg_poly = {0};
    polynom8_t rem_poly = {0};

    /* msg_poly = msg * x^deg(gen_poly) */
    msg_poly.deg = msg_len + gen_poly.deg - 1;
    msg_poly.coeffs = calloc(msg_poly.deg + 1, sizeof(uint8_t));
    memcpy(&msg_poly.coeffs[gen_poly.deg], msg, msg_len);

    poly_div(msg_poly, gen_poly, &rem_poly);
    memcpy(bchfec, rem_poly.coeffs, rem_poly.deg + 1);

    free(msg_poly.coeffs);
    free(rem_poly.coeffs);
}

void print_poly16(polynom16_t poly) {
    printf("deg: %u\n", poly.deg);
    for (int i = 0; i < poly.deg+1; i++) {
        printf("%u ", poly.coeffs[i]);
    }
    printf("\n");
}

void berlekamp_massey(uint16_t *syndromes, polynom16_t *C) {
    uint16_t L = 0;
    uint16_t b = 1;
    uint16_t m = 1;
    polynom16_t B = {.deg=0};
    B.coeffs = calloc(1, sizeof(uint16_t));
    B.coeffs[0] = 1;
    C->deg = 0;
    C->coeffs = calloc(2*t+1, sizeof(uint16_t));
    C->coeffs[0] = 1;
    for (uint8_t n = 0; n < 2*t; n++) {
        uint16_t d = syndromes[n];

        for (int i = 1; i < L+1; i++) {
            if (i < C->deg+1) {
                d ^= gf_mul(C->coeffs[i], syndromes[n-i]);
            }
        }

        if (d == 0) {
            m += 1;
            continue;
        }

        polynom16_t T = {.deg = C->deg, .coeffs = NULL};
        T.coeffs = malloc((C->deg+1)*sizeof(uint16_t));
        memcpy(T.coeffs, C->coeffs, (C->deg+1)*sizeof(uint16_t));

        uint16_t coef = gf_div(d, b);
        polynom16_t correction = {.deg = m + B.deg, .coeffs = NULL};
        correction.coeffs = calloc(correction.deg+1, sizeof(uint16_t));
        for (int i = m; i < m+B.deg+1; i++) {
            correction.coeffs[i] = gf_mul(coef, B.coeffs[i-m]);
        }

        poly16_add(*C, correction, C);
        while (C->deg > 0 && C->coeffs[C->deg] == 0) {
            C->deg -= 1;
        }
        if (2 * L <= n) {
            L = n + 1 - L;
            POLY_FREE(B);
            B.coeffs = T.coeffs;
            B.deg = T.deg;
            b = d;
            m = 1;
        } else {
            free(T.coeffs);
            m += 1;
        }
        free(correction.coeffs);
    }
}

// ######################################################
// os.system("cls")

// G = []

polynom8_t g[12];
void bch_init() {
    bool b_short = 1;
    if (b_short) {
        POLY8_INIT(primitive_poly, {14,5,3,1,0});

        POLY8_INIT(g[0], {14,5,3,1,0});
        POLY8_INIT(g[1], {14,11,8,6,0});
        POLY8_INIT(g[2], {14,10,9,6,2,1,0});
        POLY8_INIT(g[3], {14,12,10,8,7,4,0});
        POLY8_INIT(g[4], {14,13,11,9,8,6,4,2,0});
        POLY8_INIT(g[5], {14,13,9,8,7,3,0});
        POLY8_INIT(g[6], {14,13,11,10,7,6,5,2,0});
        POLY8_INIT(g[7], {14,11,10,9,8,5,0});
        POLY8_INIT(g[8], {14,10,9,3,2,1,0});
        POLY8_INIT(g[9], {14,12,11,9,6,3,0});
        POLY8_INIT(g[10], {14,12,11,4,0});
        POLY8_INIT(g[11], {14,13,10,8,7,6,5,3,2,1,0});
    } else {
        POLY8_INIT(primitive_poly, {16,5,3,2,0});

        POLY8_INIT(g[0], {16,5,3,2,0});
        POLY8_INIT(g[1], {16,8,6,5,4,1,0});
        POLY8_INIT(g[2], {16,11,10,9,8,7,5,4,3,2,0});
        POLY8_INIT(g[3], {16,14,12,11,9,6,4,2,0});
        POLY8_INIT(g[4], {16,12,11,10,9,8,5,3,2,1,0});
        POLY8_INIT(g[5], {16,15,14,13,12,10,9,8,7,5,4,2,0});
        POLY8_INIT(g[6], {16,15,13,11,10,9,8,6,5,2,0});
        POLY8_INIT(g[7], {16,14,13,12,9,8,6,5,2,1,0});
        POLY8_INIT(g[8], {16,11,10,9,7,5,0});
        POLY8_INIT(g[9], {16,14,13,12,10,8,7,5,2,1,0});
        POLY8_INIT(g[10], {16,13,12,11,9,5,3,2,0});
        POLY8_INIT(g[11], {16,12,11,9,7,6,5,1,0});
    }

    t = 12;
    POLY8_DEFINE(tmp, {0});
    poly_mul(g[0], g[1], &gen_poly);
    for (int i = 2; i < t; i += 2) {
        poly_mul(gen_poly, g[i], &tmp);
        POLY_FREE(gen_poly);
        poly_mul(tmp, g[i+1], &gen_poly);
        POLY_FREE(tmp);
    }

    build_alpha_table();

    const size_t msg_len = 14232;
    uint8_t *msg = calloc(msg_len+gen_poly.deg, sizeof(uint8_t));
    msg[433] = 1;
    
    encode(msg, msg_len, &msg[msg_len]);

    polynom8_t codeword_poly = {0};
    codeword_poly.deg = msg_len + gen_poly.deg - 1;
    codeword_poly.coeffs = calloc(codeword_poly.deg + 1, sizeof(uint8_t));
    memcpy(codeword_poly.coeffs, &msg[msg_len], gen_poly.deg);
    memcpy(&codeword_poly.coeffs[gen_poly.deg], msg, msg_len);

    uint16_t *syndromes = malloc(2 * t * sizeof(uint16_t));
    int err = 123;
    codeword_poly.coeffs[err++] ^= 1;
    calc_syndromes(codeword_poly, syndromes);
    for (int i = 0; i < 2 * t; i++) {
        printf("S%d = %u\n", i + 1, syndromes[i]);
    }

    polynom16_t locator_poly = {0};
    berlekamp_massey(syndromes, &locator_poly);

    // chien
    int num_errors = 0;
    const size_t codeword_len = codeword_poly.deg + 1;
    for (int pow = 0; pow < GF_SIZE; pow++) {
        uint16_t res = 0;
        for (int p = 0; p <= locator_poly.deg; p++) {
            uint16_t coeff = locator_poly.coeffs[p];
            if (coeff) {
                res ^= alpha_to[gf_reduce_exp(index_of[coeff] - p * pow)];
            }
        }

        if (res == 0) {
            num_errors += 1;
            if ((size_t)pow < codeword_len) {
                codeword_poly.coeffs[pow] ^= 1;
                printf("err at %d\n", pow);
            } else {
                printf("root outside shortened codeword at %d\n", pow);
            }
        }
    }

    if (num_errors != locator_poly.deg) {
        printf("Decoding failure\n");
    }

    for (int pow = 1; pow < 2 * t + 1; pow++) {
        if (gf_poly_eval_alpha_power(codeword_poly, pow) != 0) {
            printf("Decoding failure\n");
            break;
        }
    }
    printf("Decoding success\n");

    free(codeword_poly.coeffs);
    free(syndromes);
}