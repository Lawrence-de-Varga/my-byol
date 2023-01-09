#include <stdio.h>


long expo(long x, long y) {
    if (y == 0) {
        return 1;
    } else if (y == 1) {
        return x;
    } else {
        return x * expo(x , (y - 1));
    }
}

long min(long x, long y) {
    if (x <= y) {
        return x;
    } else {
        return y;
    }
}

long max (long x, long y) {
    if (x >= y) {
        return x;
    } else { 
        return y;
    }
}
