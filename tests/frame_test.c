#include "matype.h"
#include "frame.h"
#include "assert.h"

void test_frame(void) {
    frame_t frame = {0};
    frame_init(&frame);

    matype_t matype = {0};
    matype.ts_gs = TS_GS_TRANSPORT;
    matype.sis_mis = SIS_MIS_SINGLE_INPUT;
    matype.ccm_acm = CCM_ACM_CCM;
    matype.issyi = ISSYI_ACTIVE;
    matype.npd = NPD_ACTIVE;
    matype.ro = RO_0_35;
    
    uint8_t matype_buf[2] = {0};
    matype_to_bytes(&matype, matype_buf);
    frame_set_matype(&frame, matype_buf);

    matype_t matype2;
    frame_get_matype(&frame, &matype2);

    assert(matype2.ts_gs == matype.ts_gs);
    assert(matype2.sis_mis == matype.sis_mis);
    assert(matype2.ccm_acm == matype.ccm_acm);
    assert(matype2.issyi == matype.issyi);
    assert(matype2.npd == matype.npd);
    assert(matype2.ro == matype.ro);
}

int main(void) {
    test_frame();
    return 0;
}
