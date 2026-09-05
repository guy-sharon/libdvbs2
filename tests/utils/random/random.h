#ifndef TESTS_UTILS_RANDOM_RANDOM_H
#define TESTS_UTILS_RANDOM_RANDOM_H

#include <stdint.h>

void random_set_seed(uint32_t seed);
uint32_t random_randu32(void);
float random_randf(void);

#endif /* TESTS_UTILS_RANDOM_RANDOM_H */
