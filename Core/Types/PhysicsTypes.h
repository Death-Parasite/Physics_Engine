#pragma once
#include <vector>

namespace Physics {

    struct alignas(16) Vec3 {
        float x, y, z, w; 

        Vec3() : x(0), y(0), z(0), w(0) {}
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(0) {}
        
        Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    };

    struct ClothParticleData {
        std::vector<Vec3> positions;          
        std::vector<Vec3> predictedPositions; 
        std::vector<Vec3> velocities;         
        std::vector<float> invMasses;         
        std::vector<int> triangles; 
        
        size_t count = 0;

        void AddParticle(const Vec3& pos, float mass) {
            positions.push_back(pos);
            predictedPositions.push_back(pos);
            velocities.push_back(Vec3(0, 0, 0));
            invMasses.push_back(mass > 0.0f ? 1.0f / mass : 0.0f);
            count++;
        }
    };

    enum class ConstraintType { Distance, Bending };

    struct Constraint {
        int particleA;
        int particleB;
        float restLength;
        float compliance; // XPBD Alpha: 0.0f = Leather (Hard), 0.0001f = Silk
        ConstraintType type;
    };
}