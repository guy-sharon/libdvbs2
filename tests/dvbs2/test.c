#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "dvbs2.h"

int main(void)
{
    assert(strcmp(dvbs2_version(), DVBS2_VERSION) == 0);
    return 0;
}
