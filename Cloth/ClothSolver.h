#pragma once
#include "../Core/Types/PhysicsTypes.h"
#include <vector>

namespace Physics {

    class ClothSolver {
    public:
        ClothSolver();
        ~ClothSolver();

        void InitializeGridMesh(int width, int height, float spacing, float totalMass);
        void PinParticle(int index); 
        void AddParticleToMesh(const Vec3& pos, float mass);
        void AddConstraint(int pA, int pB, float restLen, float stiffness, ConstraintType type);
        void AddTriangle(int p1, int p2, int p3); 

        // --- NAYA: Solid Sphere Collider API ---
        void SetSphereCollider(const Vec3& center, float radius);

        void StepSimulation(float dt, const Vec3& gravity, int subSteps);

        const ClothParticleData& GetParticleData() const { return m_Particles; }
        const std::vector<Constraint>& GetConstraints() const { return m_Constraints; }

    private:
        void SubStep(float subDt, const Vec3& gravity);
        void PredictPositions(float subDt, const Vec3& gravity);
        void SolveConstraints(float subDt);
        void SolveCollisions(); // Collision resolution
        void UpdateVelocities(float subDt);

    private:
        ClothParticleData m_Particles;
        std::vector<Constraint> m_Constraints;
        float m_Damping = 0.99f; // Silk jaisi smooth sliding ke liye 0.99

        Vec3 m_SphereCenter = Vec3(0, -9999.0f, 0);
        float m_SphereRadius = 0.0f;
    };

}