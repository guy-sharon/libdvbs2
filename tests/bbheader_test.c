#include "bbheader.h"
#include "matype.h"

#include <stdio.h>

int main(void) {
    matype_t matype = {0};
    // matype.ts_gs = TS_GS_TRANSPORT;
    matype.npd = NPD_ACTIVE;

    printf("%02x\n", *(uint8_t*)&matype);
    return 0;
}