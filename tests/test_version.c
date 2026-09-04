#include <assert.h>
#include <string.h>

#include "dvbs2/dvbs2.h"

int main(void)
{
    assert(strcmp(dvbs2_version(), "0.1.0") == 0);
    return 0;
}
