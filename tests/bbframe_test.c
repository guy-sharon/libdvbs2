#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "stdio.h"

#include "bbframe.h"

static void test_scramble_known_vector(void)
{
	uint8_t data[] = { 0x00, 0x01, 0x02, 0x03,
					   0x04, 0x05, 0x06, 0x07 };
	const uint8_t expected[] = { 0xC0, 0x6E, 0x12, 0x2F,
								 0x08, 0x18, 0xC3, 0xCE };

	bbframe_scramble(data, sizeof(data));

	assert(memcmp(data, expected, sizeof(data)) == 0);
}

static void test_scramble_empty_input(void)
{
	uint8_t data = 0xA5;

	bbframe_scramble(&data, 0);

	assert(data == 0xA5);
}

static void test_scramble_and_descramble_round_trip(void)
{
	const uint8_t original[] = { 0x47, 0x00, 0xFF, 0xA5, 0x12, 0x80 };
	uint8_t data[sizeof(original)];

	memcpy(data, original, sizeof(data));
	bbframe_scramble(data, sizeof(data));
	assert(memcmp(data, original, sizeof(data)) != 0);

	bbframe_descramble(data, sizeof(data));

	assert(memcmp(data, original, sizeof(data)) == 0);
}

static void test_scramble_initial_sequence(void) {
    bbframe_init_scramble();
    uint8_t scramble_byte = bbframe_get_next_scramble_byte();
    assert(scramble_byte == 0xC0);
}

int main(void)
{
    test_scramble_initial_sequence();
	test_scramble_known_vector();
	test_scramble_empty_input();
	test_scramble_and_descramble_round_trip();
	return 0;
}