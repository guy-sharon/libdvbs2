
#ifndef DVBS2_COMPLEX_H
#define DVBS2_COMPLEX_H

#include <stdint.h>

typedef struct {
    float real;
    float imag;
} complexf_t;

typedef struct {
    uint16_t real;
    uint16_t imag;
} complex16_t;

#endif /* DVBS2_COMPLEX_H */