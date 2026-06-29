#pragma once
#include "../Core/Types/PhysicsTypes.h"
#include "ClothConstraints.h"
#include <vector>

namespace Physics {

    struct SphereCollider {
        Vec3 center;
        float radius;
    };

    struct CapsuleCollider {
        Vec3 pA; 
        Vec3 pB; 
        float radius;
    };

    class ClothSolver {
    public:
        ClothSolver();
        ~ClothSolver();

        void InitializeGridMesh(int width, int height, float spacing, float totalMass);
        void PinParticle(int index); 
        void AddParticleToMesh(const Vec3& pos, float mass);
        void AddTriangle(int p1, int p2, int p3); 
        void MoveParticle(int index, const Vec3& delta);

        // Forward API calls to Batched Constraint Storage
        void AddStructuralConstraint(int pA, int pB, float restLen, float compliance);
        void AddShearConstraint(int pA, int pB, float restLen, float compliance);
        void AddBendingConstraint(int pA, int pB, float restLen, float compliance);

        void ClearColliders();
        void AddSphereCollider(const Vec3& center, float radius);
        void AddCapsuleCollider(const Vec3& pA, const Vec3& pB, float radius); 
        void UpdateSphereCollider(size_t index, const Vec3& center, float radius);

        void StepSimulation(float dt, const Vec3& gravity, const Vec3& wind, int subSteps);

        const ClothParticleData& GetParticleData() const { return m_Particles; }
        const ClothConstraints& GetConstraints() const { return m_ConstraintsStorage; }
        const std::vector<SphereCollider>& GetSpheres() const { return m_Spheres; }
        const std::vector<CapsuleCollider>& GetCapsules() const { return m_Capsules; } 

    private:
        void SubStep(float subDt, const Vec3& gravity, const Vec3& wind);
        void PredictPositions(float subDt, const Vec3& gravity, const Vec3& wind);
        void SolveConstraints(float subDt);
        void SolveCollisions(); 
        void UpdateVelocities(float subDt);

    private:
        ClothParticleData m_Particles;
        ClothConstraints m_ConstraintsStorage; // New Batched Motor!
        
        std::vector<SphereCollider> m_Spheres; 
        std::vector<CapsuleCollider> m_Capsules; 
        
        float m_Damping = 0.994f; 
        float m_TotalTime = 0.0f; 
    };
}