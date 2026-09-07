#ifndef DVBS2_MATYPE_H
#define DVBS2_MATYPE_H

#include <stdint.h>

// MATYPE (2 bytes): describes the input stream(s) format, 
// the type of Mode Adaptation and the transmission Roll-off factor (RO).

/*
 * TS/GS                   | SIS/MIS      | CCM/ACM  | ISSYI          | NPD            | RO
 * ------------------------|--------------|----------|----------------|----------------|---------------
 * 11 = Transport          | 1 = single   | 1 = CCM  | 1 = active     | 1 = active     | 00 = 0,35
 * 00 = Generic Packetized | 0 = multiple | 0 = ACM  | 0 = not-active | 0 = not-active | 01 = 0,25
 * 01 = Generic continuous |              |          |                |                | 10 = 0,20
 * 10 = reserved           |              |          |                |                | 11 = reserved
 */

 typedef enum {
    TS_GS_GENERIC_PACKETIZED = 0,
    TS_GS_GENERIC_CONTINUOUS = 1,
    TS_GS_RESERVED = 2,
    TS_GS_TRANSPORT = 3
} ts_gs_t;

typedef enum {
    SIS_MIS_MULTIPLE = 0,
    SIS_MIS_SINGLE = 1
} sis_mis_t;

typedef enum {
    CCM_ACM_ACM = 0,
    CCM_ACM_CCM = 1
} ccm_acm_t;

typedef enum {
    ISSYI_NOT_ACTIVE = 0,
    ISSYI_ACTIVE = 1
} issyi_t;

typedef enum {
    NPD_NOT_ACTIVE = 0,
    NPD_ACTIVE = 1
} npd_t;

typedef enum {
    RO_0_35 = 0,
    RO_0_25 = 1,
    RO_0_20 = 2,
    RO_RESERVED = 3
} ro_t;

typedef struct {
    uint8_t ts_gs : 2; // transport stream / generic stream
    uint8_t sis_mis : 1; // single input stream / multiple input stream
    uint8_t ccm_acm : 1; // constant coding modulation / adaptive coding modulation
    uint8_t issyi : 1; // if ISSYI = 1 = active, the ISSY field is inserted after UPs
    uint8_t npd : 1; // null packet detection
    uint8_t ro : 2; // transmission Roll-off factor

    uint8_t isi: 8; // Input Stream Identifier if ISSYI = 1, otherwise reserved
} matype_t;

const int matype_size = sizeof(matype_t);

#endif /* DVBS2_MATYPE_H */