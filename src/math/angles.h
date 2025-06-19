#pragma once
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace Math {
    static constexpr double TWO_PI = M_PI * 2.0;
    inline double radians(double degrees) { return degrees * (M_PI / 180.0); }
    inline double degrees(double radians) { return radians * (180.0 / M_PI); }
    // Make angle element of [-pi, pi] by wrapping it
    inline double normalizeRad(double angle) {
        angle = std::fmod(angle + M_PI, TWO_PI);
        if (angle < 0)
            return (angle + TWO_PI) - M_PI;
        return angle - M_PI;
    }

} // namespace Math
