#include <stdio.h>

#include "dvbs2/dvbs2.h"

int main(void)
{
    printf("libdvbs2 %s\n", dvbs2_version());
    return 0;
}
