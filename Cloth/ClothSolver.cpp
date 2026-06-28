#include "ClothSolver.h"
#include "../Math/VectorMath.h"

namespace Physics {

    extern void GenerateClothGrid(ClothSolver* solver, int width, int height, float spacing, float totalMass);

    ClothSolver::ClothSolver() {}
    ClothSolver::~ClothSolver() {}

    void ClothSolver::AddParticleToMesh(const Vec3& pos, float mass) {
        m_Particles.AddParticle(pos, mass);
    }

    void ClothSolver::AddConstraint(int pA, int pB, float restLen, float stiffness, ConstraintType type) {
        m_Constraints.push_back({pA, pB, restLen, stiffness, type});
    }

    void ClothSolver::AddTriangle(int p1, int p2, int p3) {
        m_Particles.triangles.push_back(p1);
        m_Particles.triangles.push_back(p2);
        m_Particles.triangles.push_back(p3);
    }

    void ClothSolver::SetSphereCollider(const Vec3& center, float radius) {
        m_SphereCenter = center;
        m_SphereRadius = radius;
    }

    void ClothSolver::InitializeGridMesh(int width, int height, float spacing, float totalMass) {
        GenerateClothGrid(this, width, height, spacing, totalMass);
    }

    void ClothSolver::PinParticle(int index) {
        if (index >= 0 && index < m_Particles.count) {
            m_Particles.invMasses[index] = 0.0f; 
        }
    }

    void ClothSolver::StepSimulation(float dt, const Vec3& gravity, int subSteps) {
        if (dt <= 0.0f || subSteps <= 0) return;
        float subDt = dt / subSteps;

        for (int i = 0; i < subSteps; ++i) {
            SubStep(subDt, gravity);
        }
    }

    void ClothSolver::SubStep(float subDt, const Vec3& gravity) {
        PredictPositions(subDt, gravity);
        SolveConstraints(subDt);
        SolveCollisions(); // <-- Yahan kapda gole se takrayega
        UpdateVelocities(subDt);
    }

    void ClothSolver::PredictPositions(float subDt, const Vec3& gravity) {
        for (size_t i = 0; i < m_Particles.count; ++i) {
            if (m_Particles.invMasses[i] == 0.0f) continue; 

            m_Particles.velocities[i] = m_Particles.velocities[i] + (gravity * subDt);
            m_Particles.predictedPositions[i] = m_Particles.positions[i] + (m_Particles.velocities[i] * subDt);
        }
    }

    void ClothSolver::SolveConstraints(float subDt) {
        for (const auto& c : m_Constraints) {
            float wA = m_Particles.invMasses[c.particleA];
            float wB = m_Particles.invMasses[c.particleB];
            float wSum = wA + wB;

            if (wSum == 0.0f) continue; 

            Vec3& pA = m_Particles.predictedPositions[c.particleA];
            Vec3& pB = m_Particles.predictedPositions[c.particleB];

            Vec3 delta = pA - pB;
            float currentLen = Math::Length(delta);
            if (currentLen == 0.0f) continue;

            float error = currentLen - c.restLength;
            Vec3 correction = (delta * (1.0f / currentLen)) * (error / wSum);

            if (wA > 0.0f) pA = pA - (correction * wA);
            if (wB > 0.0f) pB = pB + (correction * wB);
        }
    }

    // --- XPBD SPHERE COLLISION RESOLUTION ---
    void ClothSolver::SolveCollisions() {
        if (m_SphereRadius <= 0.0f) return;

        for (size_t i = 0; i < m_Particles.count; ++i) {
            if (m_Particles.invMasses[i] == 0.0f) continue;

            Vec3& p = m_Particles.predictedPositions[i];
            Vec3 delta = p - m_SphereCenter;
            float dist = Math::Length(delta);

            // Agar particle gole ke andar घुस gaya hai:
            if (dist < m_SphereRadius && dist > 0.0001f) {
                Vec3 normal = delta * (1.0f / dist);
                // Usko gole ki surface par baahar dhakel do (+ 0.02m skin thickness)
                p = m_SphereCenter + (normal * (m_SphereRadius + 0.02f));
            }
        }
    }

    void ClothSolver::UpdateVelocities(float subDt) {
        float invSubDt = 1.0f / subDt;
        for (size_t i = 0; i < m_Particles.count; ++i) {
            if (m_Particles.invMasses[i] == 0.0f) continue;

            m_Particles.velocities[i] = (m_Particles.predictedPositions[i] - m_Particles.positions[i]) * invSubDt;
            m_Particles.velocities[i] = m_Particles.velocities[i] * m_Damping;
            m_Particles.positions[i] = m_Particles.predictedPositions[i];
        }
    }
}