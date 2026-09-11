#include "matype.h"

void matype_to_bytes(const matype_t *matype, uint8_t *bytes) {
    bytes[0] = (matype->ts_gs & 0x03) |
               ((matype->sis_mis & 0x01) << 2) |
               ((matype->ccm_acm & 0x01) << 3) |
               ((matype->issyi & 0x01) << 4) |
               ((matype->npd & 0x01) << 5) |
               ((matype->ro & 0x03) << 6);

    bytes[1] = matype->isi;
}

void matype_from_bytes(const uint8_t *bytes, matype_t *matype) {
    matype->ts_gs = bytes[0] & 0x03;
    matype->sis_mis = (bytes[0] >> 2) & 0x01;
    matype->ccm_acm = (bytes[0] >> 3) & 0x01;
    matype->issyi = (bytes[0] >> 4) & 0x01;
    matype->npd = (bytes[0] >> 5) & 0x01;
    matype->ro = (bytes[0] >> 6) & 0x03;

    matype->isi = bytes[1];
}