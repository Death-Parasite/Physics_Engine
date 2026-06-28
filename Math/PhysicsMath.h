#pragma once
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace Physics {
    namespace Math {
        constexpr float PI = 3.14159265358979323846f;
        constexpr float EPSILON = 1e-6f;

        inline float DegToRad(float deg) { return deg * (PI / 180.0f); }
        inline float RadToDeg(float rad) { return rad * (180.0f / PI); }

        template<typename T>
        inline T Clamp(T val, T minVal, T maxVal) {
            return (val < minVal) ? minVal : ((val > maxVal) ? maxVal : val);
        }

        inline float Lerp(float a, float b, float t) {
            return a + t * (b - a);
        }

        inline bool IsNearlyZero(float val, float tolerance = 1e-6f) {
            return std::abs(val) <= tolerance;
        }

        inline float SafeInvSqrt(float number) {
            if (number <= EPSILON) return 0.0f;
            return 1.0f / std::sqrt(number);
        }
    }
}