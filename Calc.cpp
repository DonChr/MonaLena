#include "Calc.h"

int32_t Median5(int32_t v0, int32_t v1, int32_t v2, int32_t v3, int32_t v4) {
    int32_t t;
    if (v0 > v1) { t = v0; v0 = v1; v1 = t; }
    if (v3 > v4) { t = v3; v3 = v4; v4 = t; }
    if (v0 > v3) { t = v0; v0 = v3; v3 = t; }
    if (v1 > v4) { t = v1; v1 = v4; v4 = t; }
    if (v1 > v2) { t = v1; v1 = v2; v2 = t; }
    if (v2 > v3) { t = v2; v2 = v3; v3 = t; }
    if (v1 > v2) { t = v1; v1 = v2; v2 = t; }
    return v2;
}

/**
<summary>Calculates the Median of 9 values as a comparator network</summary>
*/
int32_t Median9(int32_t v0, int32_t v1, int32_t v2, int32_t v3, int32_t v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8) {
    int32_t t;
    if (v1 > v2) { t = v1; v1 = v2; v2 = t; }
    if (v4 > v5) { t = v4; v4 = v5; v5 = t; }
    if (v7 > v8) { t = v7; v7 = v8; v8 = t; }
    if (v0 > v1) { t = v0; v0 = v1; v1 = t; }
    if (v3 > v4) { t = v3; v3 = v4; v4 = t; }
    if (v6 > v7) { t = v6; v6 = v7; v7 = t; }
    if (v1 > v2) { t = v1; v1 = v2; v2 = t; }
    if (v4 > v5) { t = v4; v4 = v5; v5 = t; }
    if (v7 > v8) { t = v7; v7 = v8; v8 = t; }
    if (v0 > v3) { t = v0; v0 = v3; v3 = t; }
    if (v5 > v8) { t = v5; v5 = v8; v8 = t; }
    if (v4 > v7) { t = v4; v4 = v7; v7 = t; }
    if (v3 > v6) { t = v3; v3 = v6; v6 = t; }
    if (v1 > v4) { t = v1; v1 = v4; v4 = t; }
    if (v2 > v5) { t = v2; v2 = v5; v5 = t; }
    if (v4 > v7) { t = v4; v4 = v7; v7 = t; }
    if (v4 > v2) { t = v4; v4 = v2; v2 = t; }
    if (v6 > v4) { t = v6; v6 = v4; v4 = t; }
    if (v4 > v2) { t = v4; v4 = v2; v2 = t; }
    return v4;
}
