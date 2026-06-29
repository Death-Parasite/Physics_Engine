#include "ClothSolver.h"
#include "ClothMesh.h"
#include "../Math/PhysicsMath.h"
#include "../Math/VectorMath.h"
#include <omp.h>

namespace Physics {

    ClothSolver::ClothSolver() {}
    ClothSolver::~ClothSolver() {}

    void ClothSolver::AddParticleToMesh(const Vec3& pos, float mass) {
        m_Particles.AddParticle(pos, mass);
    }

    void ClothSolver::AddTriangle(int p1, int p2, int p3) {
        m_Particles.triangles.push_back(p1);
        m_Particles.triangles.push_back(p2);
        m_Particles.triangles.push_back(p3);
    }

    void ClothSolver::AddStructuralConstraint(int pA, int pB, float restLen, float compliance) {
        m_ConstraintsStorage.AddStructural(pA, pB, restLen, compliance);
    }

    void ClothSolver::AddShearConstraint(int pA, int pB, float restLen, float compliance) {
        m_ConstraintsStorage.AddShear(pA, pB, restLen, compliance);
    }

    void ClothSolver::AddBendingConstraint(int pA, int pB, float restLen, float compliance) {
        m_ConstraintsStorage.AddBending(pA, pB, restLen, compliance);
    }

    void ClothSolver::MoveParticle(int index, const Vec3& delta) {
        if(index >= 0 && index < m_Particles.count) {
            m_Particles.positions[index] = m_Particles.positions[index] + delta;
            m_Particles.predictedPositions[index] = m_Particles.predictedPositions[index] + delta;
        }
    }

    void ClothSolver::ClearColliders() {
        m_Spheres.clear();
        m_Capsules.clear(); 
    }

    void ClothSolver::AddSphereCollider(const Vec3& center, float radius) {
        m_Spheres.push_back({center, radius});
    }

    void ClothSolver::UpdateSphereCollider(size_t index, const Vec3& center, float radius) {
        if (index < m_Spheres.size()) {
            m_Spheres[index].center = center;
            m_Spheres[index].radius = radius;
        }
    }

    void ClothSolver::AddCapsuleCollider(const Vec3& pA, const Vec3& pB, float radius) {
        m_Capsules.push_back({pA, pB, radius});
    }

    void ClothSolver::InitializeGridMesh(int width, int height, float spacing, float totalMass) {
        m_ConstraintsStorage.ClearAll();
        GenerateClothGrid(this, width, height, spacing, totalMass);
    }

    void ClothSolver::PinParticle(int index) {
        if (index >= 0 && index < m_Particles.count) {
            m_Particles.invMasses[index] = 0.0f; 
        }
    }

    void ClothSolver::StepSimulation(float dt, const Vec3& gravity, const Vec3& wind, int subSteps) {
        if (dt <= 0.0f || subSteps <= 0) return;
        float subDt = dt / subSteps;
        m_TotalTime += dt;

        m_ConstraintsStorage.PreSolve(subDt);

        for (int i = 0; i < subSteps; ++i) {
            SubStep(subDt, gravity, wind);
        }
    }

    void ClothSolver::SubStep(float subDt, const Vec3& gravity, const Vec3& wind) {
        PredictPositions(subDt, gravity, wind);
        SolveConstraints(subDt);
        SolveCollisions(); 
        UpdateVelocities(subDt);
    }

    // --- THE 90+ FPS UNLOCKED PREDICTOR (Raw __restrict Pointers) ---
    void ClothSolver::PredictPositions(float subDt, const Vec3& gravity, const Vec3& wind) {
        int cnt = static_cast<int>(m_Particles.count);
        Vec3* __restrict pos = m_Particles.positions.data();
        Vec3* __restrict pred = m_Particles.predictedPositions.data();
        Vec3* __restrict vel = m_Particles.velocities.data();
        const float* __restrict invM = m_Particles.invMasses.data();
        float timeLocal = m_TotalTime;

        #pragma omp parallel for simd schedule(static)
        for (int i = 0; i < cnt; ++i) {
            if (invM[i] == 0.0f) continue; 

            float wave = std::sin(timeLocal * 6.0f + (pos[i].x * 4.0f));
            Vec3 aeroForce = wind * (wave * 0.35f + 0.65f);

            vel[i] = vel[i] + ((gravity + aeroForce) * subDt);
            pred[i] = pos[i] + (vel[i] * subDt);
        }
    }

    void ClothSolver::SolveConstraints(float subDt) {
        m_ConstraintsStorage.Solve(m_Particles, subDt);
    }

    // --- EXTREME SIMD COLLISION PIPELINE ---
    void ClothSolver::SolveCollisions() {
        int cnt = static_cast<int>(m_Particles.count);
        Vec3* __restrict pos = m_Particles.positions.data();
        Vec3* __restrict pred = m_Particles.predictedPositions.data();
        const float* __restrict invM = m_Particles.invMasses.data();

        int sSize = static_cast<int>(m_Spheres.size());
        const SphereCollider* __restrict spheres = m_Spheres.data();

        int cSize = static_cast<int>(m_Capsules.size());
        const CapsuleCollider* __restrict caps = m_Capsules.data();

        #pragma omp parallel for simd schedule(static)
        for (int i = 0; i < cnt; ++i) {
            if (invM[i] == 0.0f) continue;
            Vec3 p = pred[i];
            bool hit = false;
            
            for (int s = 0; s < sSize; ++s) {
                Vec3 delta = p - spheres[s].center;
                float dist = Math::Length(delta);
                float gap = spheres[s].radius + 0.012f; 
                if (dist < gap && dist > 1e-6f) {
                    p = spheres[s].center + (delta * (gap / dist));
                    hit = true;
                }
            }

            for (int c = 0; c < cSize; ++c) {
                Vec3 ab = caps[c].pB - caps[c].pA;
                Vec3 ap = p - caps[c].pA;
                float ab2 = Math::Dot(ab, ab);
                float t = (ab2 > 1e-6f) ? (Math::Dot(ap, ab) / ab2) : 0.0f;
                t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
                
                Vec3 closest = caps[c].pA + (ab * t);
                Vec3 delta = p - closest;
                float dist = Math::Length(delta);
                float gap = caps[c].radius + 0.012f;

                if (dist < gap && dist > 1e-6f) {
                    p = closest + (delta * (gap / dist));
                    hit = true;
                }
            }

            if(hit) {
                pred[i] = p;
                pos[i] = p - ((p - pos[i]) * 0.15f); // High grip
            }
        }
    }

    void ClothSolver::UpdateVelocities(float subDt) {
        float invSubDt = 1.0f / subDt;
        int cnt = static_cast<int>(m_Particles.count);
        Vec3* __restrict pos = m_Particles.positions.data();
        Vec3* __restrict pred = m_Particles.predictedPositions.data();
        Vec3* __restrict vel = m_Particles.velocities.data();
        const float* __restrict invM = m_Particles.invMasses.data();

        #pragma omp parallel for simd schedule(static)
        for (int i = 0; i < cnt; ++i) {
            if (invM[i] == 0.0f) continue;
            vel[i] = (pred[i] - pos[i]) * invSubDt;
            vel[i] = vel[i] * m_Damping;
            pos[i] = pred[i];
        }
    }
}