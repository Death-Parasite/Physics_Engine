#include "ClothConstraints.h"
#include "../Math/VectorMath.h"
#include "../Math/PhysicsMath.h"

namespace Physics {

    ClothConstraints::ClothConstraints() {}
    ClothConstraints::~ClothConstraints() {}

    void ClothConstraints::AddStructural(int pA, int pB, float restLen, float compliance) {
        m_Structural.AddConstraint(pA, pB, restLen, compliance);
    }

    void ClothConstraints::AddShear(int pA, int pB, float restLen, float compliance) {
        m_Shear.AddConstraint(pA, pB, restLen, compliance);
    }

    void ClothConstraints::AddBending(int pA, int pB, float restLen, float compliance) {
        m_Bending.AddConstraint(pA, pB, restLen, compliance);
    }

    void ClothConstraints::ClearAll() {
        m_Structural.Clear();
        m_Shear.Clear();
        m_Bending.Clear();
    }

    void ClothConstraints::PreSolve(float dt) {
        // Har naye frame se pehle purane lambdas ko zero karna zaroori hai
        // taaki XPBD fresh force calculate kare.
        for(size_t i = 0; i < m_Structural.count; ++i) m_Structural.lambda[i] = 0.0f;
        for(size_t i = 0; i < m_Shear.count; ++i) m_Shear.lambda[i] = 0.0f;
        for(size_t i = 0; i < m_Bending.count; ++i) m_Bending.lambda[i] = 0.0f;
    }

    void ClothConstraints::Solve(ClothParticleData& particles, float dt) {
        if (dt <= 0.0f) return;
        
        // XPBD Requires inverse time squared for alpha compliance
        float invDtSq = 1.0f / (dt * dt);

        // Solve order matters for stability!
        // 1. Structural (Main shape hold karta hai)
        SolveBatch(m_Structural, particles, invDtSq);
        // 2. Shear (Diagonals ko stretch hone se rokti hai)
        SolveBatch(m_Shear, particles, invDtSq);
        // 3. Bending (Folds aur wrinkles banati hai)
        SolveBatch(m_Bending, particles, invDtSq);
    }

    // --- HIGHLY OPTIMIZED XPBD SOLVER LOOP ---
    void ClothConstraints::SolveBatch(ConstraintBatch& batch, ClothParticleData& particles, float invDtSq) {
        for (size_t i = 0; i < batch.count; ++i) {
            int idxA = batch.pA[i];
            int idxB = batch.pB[i];

            float wA = particles.invMasses[idxA];
            float wB = particles.invMasses[idxB];
            float wSum = wA + wB;

            // Agar dono particles pinned (wSum == 0) hain, toh skip karo (Save CPU cycles)
            if (wSum <= 1e-6f) continue;

            Vec3& pA = particles.predictedPositions[idxA];
            Vec3& pB = particles.predictedPositions[idxB];

            Vec3 delta = pA - pB;
            float currentLen = Math::Length(delta);
            
            // Zero length crash prevention (Math Safety)
            if (currentLen <= 1e-7f) continue;

            float restLen = batch.restLength[i];
            float compliance = batch.compliance[i];
            
            // Equation: C(x) = |x1 - x2| - d
            float C = currentLen - restLen;
            
            // Calculate Alpha: compliance / dt^2
            float alpha = compliance * invDtSq;

            // XPBD Lagrange Multiplier (Delta Lambda)
            // Equation: dLambda = (-C - alpha * lambda) / (wSum + alpha)
            float& lambda = batch.lambda[i];
            float dLambda = (-C - alpha * lambda) / (wSum + alpha);
            
            // Accumulate lambda
            lambda += dLambda;

            // Apply position correction
            // delta / currentLen is the normalized gradient (direction)
            Vec3 correction = (delta * (1.0f / currentLen)) * dLambda;

            if (wA > 0.0f) pA = pA + (correction * wA);
            if (wB > 0.0f) pB = pB - (correction * wB);
        }
    }
}