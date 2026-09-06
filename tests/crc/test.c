#include <assert.h>
#include <stdint.h>

#include "crc.h"

static void test_crc8_empty_and_single_bytes(void)
{
    uint8_t zero = 0x00;
    uint8_t one = 0x01;
    uint8_t ff = 0xFF;

    assert(crc8(&zero, 0) == 0x00);
    assert(crc8(&zero, 1) == 0x00);
    assert(crc8(&one, 1) == CRC8_POLYNOMIAL);
    assert(crc8(&ff, 1) == 0xAA);
}

static void test_crc8_sequences(void)
{
    uint8_t data_1[] = "123456789";
    uint8_t data_2[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                         0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    uint8_t data_3[] = "abc";

    assert(crc8(data_1, sizeof(data_1) - 1) == 0x64);
    assert(crc8(data_2, sizeof(data_2)) == 0x00);
    assert(crc8(data_3, sizeof(data_3) - 1) == 0xCA);
}

int main(void)
{
    test_crc8_empty_and_single_bytes();
    test_crc8_sequences();
    return 0;
}
