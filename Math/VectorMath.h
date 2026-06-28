#pragma once
#include "../Core/Types/PhysicsTypes.h"
#include <cmath>

namespace Physics {
    namespace Math {
        
        inline float Length(const Vec3& v) {
            return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        inline float Distance(const Vec3& a, const Vec3& b) {
            return Length(a - b);
        }

        inline float Dot(const Vec3& a, const Vec3& b) {
            return (a.x * b.x + a.y * b.y + a.z * b.z);
        }

    }
}