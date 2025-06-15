#pragma once
#include <stdint.h>
#include <cmath>

namespace Math {
    struct Vec2 {
        float x, y;

        Vec2() : x(0), y(0) {}
        Vec2(float _x, float _y) : x(_x), y(_y) {}
        Vec2(int16_t _x, int16_t _y) : x(static_cast<float>(_x)), y(static_cast<float>(_y)) {}
        
        Vec2 operator+(Vec2 const& other) {
            return Vec2(this->x + other.x, this->y + other.y);
        }
        Vec2 operator-(Vec2 const& other) {
            return Vec2(this->x - other.x, this->y - other.y);
        }
        Vec2 operator*(Vec2 const& other) {
            return Vec2(this->x * other.x, this->y * other.y);
        }
        Vec2 operator*(float const& other) {
            return Vec2(this->x * other, this->y * other);
        }
        Vec2 operator/(Vec2 const& other) {
            return Vec2(this->x / other.x, this->y / other.y);
        }
    };

    inline float dot(const Vec2& a, const Vec2& b) {
        return a.x * b.x + a.y * b.y;
    }
    inline float length2(const Vec2& a) {
        return a.x*a.x + a.y*a.y;
    }
    inline float length(const Vec2& a) {
        return std::sqrt(Math::length2(a));
    }
    
} // namespace Math
