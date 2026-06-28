#pragma once
#include <vector>

// Forward declaring SIMD vector types (Humare Math module se aayenge)
// Abhi ke liye standard float arrays use kar rahe hain alignment ke sath
namespace Physics {

    struct alignas(16) Vec3 {
        float x, y, z, w; // 'w' padding hai 16-byte SIMD alignment ke liye

        Vec3() : x(0), y(0), z(0), w(0) {}
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(0) {}
        
        Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    };

    // XPBD ka core principle: Pinned/Static particles ka inverseMass = 0.0f hota hai
 struct ClothParticleData {
        std::vector<Vec3> positions;          
        std::vector<Vec3> predictedPositions; 
        std::vector<Vec3> velocities;         
        std::vector<float> invMasses;         
        
        // --- NAYA ADD HUA: 3D Triangles ke corner indices ---
        std::vector<int> triangles; // Har 3 number = 1 Triangle face
        // ---------------------------------------------------

        size_t count = 0;

        void AddParticle(const Vec3& pos, float mass) {
            positions.push_back(pos);
            predictedPositions.push_back(pos);
            velocities.push_back(Vec3(0, 0, 0));
            invMasses.push_back(mass > 0.0f ? 1.0f / mass : 0.0f);
            count++;
        }
    };

    enum class ConstraintType {
        Distance,   // Kapda stretch hone se rokne ke liye (Structural & Shear)
        Bending,    // Kapde me natural folds aur wrinkles ke liye (Isometric Bending)
        Volume      // Inflatable ya soft balloons ke liye
    };

    struct Constraint {
        int particleA;
        int particleB;
        float restLength;
        float stiffness; // XPBD compliance (alpha) factor
        ConstraintType type;
    };

}