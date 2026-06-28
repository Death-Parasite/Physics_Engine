#pragma once
#include "../Core/Types/PhysicsTypes.h"
#include <cmath>

namespace Physics {
    struct Quat {
        float x, y, z, w;

        Quat() : x(0), y(0), z(0), w(1) {}
        Quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        Quat Conjugate() const { return Quat(-x, -y, -z, w); }

        void Normalize() {
            float len = std::sqrt(x*x + y*y + z*z + w*w);
            if (len > 1e-6f) {
                float inv = 1.0f / len;
                x *= inv; y *= inv; z *= inv; w *= inv;
            }
        }
    };
}