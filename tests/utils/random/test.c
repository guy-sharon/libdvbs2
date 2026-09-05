#include <assert.h>
#include <stdio.h>

#include "random.h"

static void test_random_number_generator(void)
{
    enum { SAMPLE_COUNT = 32 };
    uint32_t integers[SAMPLE_COUNT];
    float floats[SAMPLE_COUNT];
    int has_different_integer = 0;

    /* The same seed must reproduce the same integer and float sequence. */
    random_set_seed(12345);
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        integers[i] = random_randu32();
    }
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        floats[i] = random_randf();
        assert(floats[i] >= 0.0f);
        assert(floats[i] < 1.0f);
    }

    random_set_seed(12345);
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        uint32_t value = random_randu32();
        assert(value == integers[i]);
        if (value != integers[0]) {
            has_different_integer = 1;
        }
    }
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        float value = random_randf();
        assert(value == floats[i]);
        assert(value >= 0.0f);
        assert(value < 1.0f);
    }

    /* A zero seed is intentionally normalized to the valid seed value 1. */
    random_set_seed(0);
    uint32_t zero_seed_value = random_randu32();
    random_set_seed(1);
    assert(random_randu32() == zero_seed_value);

    assert(has_different_integer);
}

int main(void)
{
    test_random_number_generator();
    return 0;
}
