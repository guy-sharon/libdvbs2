#include "bch.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "assert.h"
#include "stdio.h"

// *********************************************************************** //
// ******************************** Types ******************************** //
// *********************************************************************** //
typedef struct {
    uint8_t *coeffs;
    uint16_t deg;
} polynom8_t;

typedef struct {
    uint16_t *coeffs;
    uint16_t deg;
} polynom16_t;

// *********************************************************************** //
// ******************************* Statics ******************************* //
// *********************************************************************** //
static uint16_t *alpha_to; // alpha_to[m] = a^m, where a is the primitive element
static uint16_t *index_of; // index_of[x] = m, where x = a^m
static uint16_t GF_SIZE = 0;

static polynom8_t primitive_poly = {0};
static polynom8_t gen_poly = {0};

static modcod_t modcod;
static size_t parity_len = 0; // number of parity bits
static polynom16_t locator_poly;

// *********************************************************************** //
// ******************************* Defines ******************************* //
// *********************************************************************** //
#define MAX_BCH_T       12

#define POLY_FREE(poly) \
    do { \
        free((poly).coeffs); \
        (poly).coeffs = NULL; \
    } while (0)

#define POLY_INIT(name, coeffsize, ...) \
    { \
        uint8_t ones[] = __VA_ARGS__; \
        (name).deg = ones[0]; \
        (name).coeffs = calloc(ones[0]+1, coeffsize); \
        for (long unsigned int i = 0; i < sizeof(ones)/sizeof(ones[0]); i++) { \
            (name).coeffs[ones[i]] = 1; \
        } \
    }

#define POLY8_INIT(name, ...)   POLY_INIT(name, sizeof(uint8_t), __VA_ARGS__)
#define POLY16_INIT(name, ...)  POLY_INIT(name, sizeof(uint16_t), __VA_ARGS__)

#define POLY16_DEFINE(name, ...) \
    polynom16_t name; \
    POLY16_INIT(name, __VA_ARGS__); 

// **************************************************************************** //
// ************************* Galois Field Arithmetics ************************* //
// **************************************************************************** //
static uint16_t gf_reduce_exp(int exponent) {
    int reduced = exponent % GF_SIZE;
    return (uint16_t)(reduced < 0 ? reduced + GF_SIZE : reduced);
}

static uint16_t gf_mul(uint16_t a, uint16_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    return alpha_to[(index_of[a] + index_of[b]) % GF_SIZE];
}

static uint16_t gf_div(uint16_t a, uint16_t b) {
    if (a == 0) {
        return 0;
    }
    return alpha_to[gf_reduce_exp(index_of[a] - index_of[b])];
}

static uint16_t arr_to_gf(uint8_t *arr, size_t len) {
    uint16_t gf = 0;
    for (size_t i = 0; i < len; i++) {
        gf += arr[i] * (1<<i);
    }
    return gf;
}

static uint16_t gf_poly8_eval_alpha_power(polynom8_t poly, uint16_t power) {
    uint16_t res = 0;
    for (int i = 0; i < poly.deg+1; i++) {
        if (poly.coeffs[i]) {
            res ^= alpha_to[(i*power) % GF_SIZE];
        }
    }
    return res;
}

static uint16_t gf_poly16_eval_alpha_power(polynom16_t poly, uint16_t power) {
    uint16_t res = 0;
    for (int p = 0; p <= poly.deg; p++) {
        uint16_t coeff = poly.coeffs[p];
        if (coeff) {
            res ^= alpha_to[gf_reduce_exp(index_of[coeff] + p * power)];
        }
    }
    return res;
}

// **************************************************************************** //
// *************************** Polynom Arithmetics **************************** //
// **************************************************************************** //
static void lsfr_step(uint8_t *lsfr, bool f_in, polynom8_t poly) {
    // taken from NASA paper 19670030021
    uint16_t lsfr_len = poly.deg;
    bool out = lsfr[lsfr_len-1];
    for (int i = lsfr_len-1; i > 0; i--) {
        lsfr[i] = lsfr[i-1] ^ (out & poly.coeffs[i]);
    }
    lsfr[0] = (out & poly.coeffs[0]) ^ f_in;
}

static void poly8_div(polynom8_t poly_nom, polynom8_t poly_den, 
                      uint16_t m, polynom8_t *out) {
    // out = (x^m*poly_nom) % poly_den
    const size_t n = poly_nom.deg;
    if (out->coeffs == NULL) {
        out->coeffs = calloc(poly_den.deg, sizeof(uint8_t));
    }
    memset(out->coeffs, 0, poly_den.deg * sizeof(uint8_t));
    out->deg = poly_den.deg - 1;
    for (size_t i = 0; i < n+1; i++) {
        bool f_in = poly_nom.coeffs[n-i];
        lsfr_step(out->coeffs, f_in, poly_den);
    }
    for (size_t i = 0; i < m; i++) {
        lsfr_step(out->coeffs, 0, poly_den);
    }
}

static void poly8_mul(polynom8_t poly1, polynom8_t poly2, polynom8_t *out) {
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

// static void poly8_add(polynom8_t poly1, polynom8_t poly2, polynom8_t *out) {
//     uint16_t deg = poly1.deg > poly2.deg ? poly1.deg : poly2.deg;
//     if (out->coeffs == NULL) {
//         out->coeffs = calloc(deg+1, sizeof(uint8_t));
//     }
//     out->deg = deg;
//     for (int i = 0; i < deg+1; i++) {
//         bool v1 = i > poly1.deg ? 0 : poly1.coeffs[i];
//         bool v2 = i > poly2.deg ? 0 : poly2.coeffs[i];
//         out->coeffs[i] = v1 ^ v2;
//     }
// }

static void poly16_add(polynom16_t poly1, polynom16_t poly2, polynom16_t *out) {
    uint16_t deg = poly1.deg > poly2.deg ? poly1.deg : poly2.deg;
    polynom16_t tmp = {.deg = deg, .coeffs = calloc(deg + 1, sizeof(uint16_t))};

    for (int i = 0; i < deg + 1; i++) {
        uint16_t v1 = i > poly1.deg ? 0 : poly1.coeffs[i];
        uint16_t v2 = i > poly2.deg ? 0 : poly2.coeffs[i];
        tmp.coeffs[i] = v1 ^ v2;
    }

    free(out->coeffs);
    out->coeffs = tmp.coeffs;
    out->deg = deg;
}

// **************************************************************************** //
// ********************************* Encoding ********************************* //
// **************************************************************************** //
void bch_encode(uint8_t *frame) {
    if (frame == NULL) {
        return;
    }

    polynom8_t data_poly = {.deg = modcod.kbch - 1, .coeffs = frame};
    polynom8_t rem_poly = {.coeffs = &frame[modcod.kbch]};
    poly8_div(data_poly, gen_poly, parity_len, &rem_poly);
}

// **************************************************************************** //
// ********************************* Decoding ********************************* //
// **************************************************************************** //
static uint16_t calc_syndrome(polynom8_t data_poly, 
                              polynom8_t rem_poly, uint16_t power) {
    uint16_t syndrome = gf_poly8_eval_alpha_power(rem_poly, power);
    syndrome ^= gf_mul(alpha_to[(power*parity_len) % GF_SIZE],
                gf_poly8_eval_alpha_power(data_poly, power));
    return syndrome;
}
static void calc_syndromes(polynom8_t data_poly, 
                           polynom8_t rem_poly, uint16_t *syndromes) {
    for (uint16_t power = 1; power < 2*modcod.bch_t+1; power++) {
        syndromes[power-1] = calc_syndrome(data_poly, rem_poly, power);
    }
}

static void berlekamp_massey(uint16_t *syndromes, polynom16_t *C) {
    uint16_t L = 0, b = 1, m = 1;
    POLY16_DEFINE(B, {0}); // B(x) = 1
    POLY16_INIT(*C, {0}); // C(x) = 1
    for (uint8_t n = 0; n < 2*modcod.bch_t; n++) {
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
            POLY_FREE(T);
            m += 1;
        }
        POLY_FREE(correction);
    }
    POLY_FREE(B);
}

static void correct_errors(uint8_t *data, int *error_positions, int num_errors) {
    for (int i = 0; i < num_errors; i++) {
        data[error_positions[i]] ^= 1;
    }
}

int locate_errors(polynom16_t locator_poly, int *error_positions) {
    int num_errors = 0;
    for (uint16_t p = 0; p < modcod.nbch; p++) {
        if (gf_poly16_eval_alpha_power(locator_poly, GF_SIZE - p) == 0) {
            error_positions[num_errors++] = p < parity_len ? p + modcod.kbch : 
                                                             p - parity_len;
            if (num_errors > modcod.bch_t) {
                return -1;
            }
        }
    }
    return num_errors;
}

bool bch_decode(uint8_t *frame) {
    polynom8_t data_poly = {.deg = modcod.kbch - 1, .coeffs = frame};
    polynom8_t rem_poly = {.deg = parity_len - 1, .coeffs = &frame[modcod.kbch]};
    memset(&locator_poly, 0, sizeof(locator_poly));
    uint16_t syndromes[2 * MAX_BCH_T] = {0};
    int error_positions[MAX_BCH_T] = {-1};

    if (frame == NULL) {
        return false;
    }

    calc_syndromes(data_poly, rem_poly, syndromes);
    berlekamp_massey(syndromes, &locator_poly);
    int num_errors = locate_errors(locator_poly, error_positions);

    if (num_errors != locator_poly.deg) {
        return false;
    }

    correct_errors(frame, error_positions, num_errors);
    for (uint16_t p = 1; p < 2 * modcod.bch_t + 1; p++) {
        if (calc_syndrome(data_poly, rem_poly, p) != 0) {
            // recover original frame (to avoid increasing num errors)
            correct_errors(frame, error_positions, num_errors);
            return false;
        }
    }

    return true;
}


static void build_alpha_table() {
    GF_SIZE = (1<<primitive_poly.deg)-1;
    alpha_to = malloc(GF_SIZE*sizeof(uint16_t));
    index_of = malloc((GF_SIZE+1)*sizeof(uint16_t));

    uint8_t *lsfr = calloc(primitive_poly.deg, sizeof(uint8_t));
    lsfr[0] = 1;
    for (int power = 0; power < GF_SIZE; power++) {
        alpha_to[power] = arr_to_gf(lsfr, primitive_poly.deg);
        index_of[alpha_to[power]] = power;
        lsfr_step(lsfr, 0, primitive_poly);
    }
    free(lsfr);
}

size_t bch_parity_bytes(void) {
    return parity_len;
}

static void bch_free(void) {
    POLY_FREE(primitive_poly);
    POLY_FREE(gen_poly);
    free(alpha_to);
    free(index_of);
    alpha_to = NULL;
    index_of = NULL;
    modcod = (modcod_t){0};
    parity_len = 0;
}

static void build_generator_poly() {
    polynom8_t g[12];
    if (modcod.short_frame) {
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

    polynom8_t tmp = {0};
    poly8_mul(g[0], g[1], &gen_poly);
    for (int i = 2; i < modcod.bch_t; i += 2) {
        poly8_mul(gen_poly, g[i], &tmp);
        POLY_FREE(gen_poly);
        poly8_mul(tmp, g[i+1], &gen_poly);
        POLY_FREE(tmp);
    }
    assert(gen_poly.deg == parity_len);
    
    for (size_t i = 0; i < sizeof(g) / sizeof(g[0]); i++) {
        POLY_FREE(g[i]);
    }
}

void bch_init(modcod_t _modcod) {
    bch_free();

    modcod = _modcod;
    parity_len = modcod.nbch - modcod.kbch;
    
    build_generator_poly();
    build_alpha_table();
}