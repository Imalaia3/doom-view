// Simple (kind of) functions that deal with projection math.
#pragma once
#include "vector.h"

namespace Math {
    inline float pointToAngle(Math::Vec2 point, Math::Vec2 viewpos) {
        return std::atan2(point.y - viewpos.y , point.x - viewpos.x);
    }
    inline int32_t viewAngleToX(float angle, float clipangle, int32_t viewwidth) {
        // could also be 1 - tan(angle)/tan(clipangle) but that makes it more difficult to
        // understand that when angle -FOV/2 the result is 0.
        return ((viewwidth-1)/2)*(1.0f + tan(angle)/(tan(-clipangle)));
    }
} // namespace Math
