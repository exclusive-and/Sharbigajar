
#include <stdio.h>

#include "FixedPrecision.h"



int main (void) {
    FixedPrec a = {-1, 16, 0x8000000000000000};
    printf ("fixed-prec to float: %f\n", fpToFloat (a));

    return 0;
}
