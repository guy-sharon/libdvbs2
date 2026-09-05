#include "random.h"

static uint32_t state = 1;

void random_set_seed(uint32_t seed)
{
    state = seed ? seed : 1;
}

uint32_t random_randu32(void)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

float random_randf(void)
{
    return (float)random_randu32() / (float)(1ULL << 32);
}
