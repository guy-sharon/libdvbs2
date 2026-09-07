#include <assert.h>
#include <stdint.h>

#include "complex.h"

static void test_complexf_components(void)
{
    complexf_t value = { .real = 1.25f, .imag = -0.5f };

    assert(value.real == 1.25f);
    assert(value.imag == -0.5f);
}

static void test_complex16_components(void)
{
    complex16_t value = { .real = UINT16_MAX, .imag = 0 };

    assert(value.real == UINT16_MAX);
    assert(value.imag == 0);
}

int main(void)
{
    test_complexf_components();
    test_complex16_components();
    return 0;
}
