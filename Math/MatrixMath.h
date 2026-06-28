#pragma once
#include "../Core/Types/PhysicsTypes.h"

namespace Physics {
    struct Mat3 {
        float m[3][3];

        Mat3() { Identity(); }

        void Identity() {
            m[0][0]=1.0f; m[0][1]=0.0f; m[0][2]=0.0f;
            m[1][0]=0.0f; m[1][1]=1.0f; m[1][2]=0.0f;
            m[2][0]=0.0f; m[2][1]=0.0f; m[2][2]=1.0f;
        }

        Vec3 operator*(const Vec3& v) const {
            return Vec3(
                m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
                m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
                m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
            );
        }
    };
}