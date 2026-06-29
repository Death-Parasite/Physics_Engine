#pragma once
#include "../Core/Types/PhysicsTypes.h"
#include <cmath>

namespace Physics {
    namespace Math {
        
        inline __attribute__((always_inline)) float Length(const Vec3& v) {
            return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        inline __attribute__((always_inline)) float Distance(const Vec3& a, const Vec3& b) {
            return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
        }

        inline __attribute__((always_inline)) float Dot(const Vec3& a, const Vec3& b) {
            return (a.x * b.x + a.y * b.y + a.z * b.z);
        }

        inline __attribute__((always_inline)) Vec3 Cross(const Vec3& a, const Vec3& b) {
            return Vec3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }

        inline __attribute__((always_inline)) Vec3 Normalize(const Vec3& v) {
            float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            if (len > 1e-6f) {
                float inv = 1.0f / len;
                return Vec3(v.x * inv, v.y * inv, v.z * inv);
            }
            return Vec3(0, 0, 0);
        }
    }
}