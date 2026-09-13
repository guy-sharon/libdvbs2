#include <assert.h>
#include <string.h>

#include "matype.h"
#include "frame.h"

void test_matype(frame_t *framePtr) {
    matype_t matype = {0};
    matype.ts_gs = TS_GS_TRANSPORT;
    matype.sis_mis = SIS_MIS_SINGLE_INPUT;
    matype.ccm_acm = CCM_ACM_CCM;
    matype.issyi = ISSYI_ACTIVE;
    matype.npd = NPD_ACTIVE;
    matype.ro = RO_0_35;
    
    uint8_t matype_buf[2] = {0};
    matype_to_bytes(&matype, matype_buf);
    frame_set_matype(framePtr, matype_buf);

    matype_t matype2;
    frame_get_matype(framePtr, &matype2);

    assert(matype2.ts_gs == matype.ts_gs);
    assert(matype2.sis_mis == matype.sis_mis);
    assert(matype2.ccm_acm == matype.ccm_acm);
    assert(matype2.issyi == matype.issyi);
    assert(matype2.npd == matype.npd);
    assert(matype2.ro == matype.ro);
}

void test_scramble(frame_t *framePtr) {
    uint8_t original[] = { 0x00, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x06, 0x07 };
    const uint8_t expected[] = { 0xC0, 0x6E, 0x12, 0x2F,
                                 0x08, 0x18, 0xC3, 0xCE };
    size_t len = sizeof(original);

    memcpy(framePtr->buffer, original, len);
        
    frame_scramble(framePtr);
    assert(memcmp(framePtr->buffer, expected, len) == 0);

    frame_scramble(framePtr);
    assert(memcmp(framePtr->buffer, original, len) == 0);
}

void test_frame(void) {
    frame_t frame = {0};
    modcod_t modcod = SHORT_8_9;

    frame_init(&frame, modcod);

    test_matype(&frame);
    test_scramble(&frame);
    
}

int main(void) {
    test_frame();
    return 0;
}
