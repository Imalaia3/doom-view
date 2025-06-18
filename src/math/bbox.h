#pragma once
#include <stdint.h>

namespace Math {
    struct BoundingBox {
        BoundingBox() {};
        BoundingBox(int16_t xlow, int16_t ylow, int16_t xhigh, int16_t yhigh) : x1(xlow), y1(ylow), x2(xhigh), y2(yhigh) {};
        BoundingBox(float xlow, float ylow, float xhigh, float yhigh) : x1(xlow), y1(ylow), x2(xhigh), y2(yhigh) {};

        float x1, y1;
        float x2, y2;
    };
}