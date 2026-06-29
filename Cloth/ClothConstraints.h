#pragma once
#include "../Core/Types/PhysicsTypes.h"
#include <vector>

namespace Physics {

    // XPBD Constraint Data Layout (Data-Oriented Design)
    // Structure of Arrays (SoA) layout for Maximum Cache Coherency
    struct ConstraintBatch {
        std::vector<int> pA;
        std::vector<int> pB;
        std::vector<float> restLength;
        std::vector<float> compliance;
        std::vector<float> lambda; // Cache for Lagrange Multiplier (Crucial for 6k faces)
        size_t count = 0;

        void AddConstraint(int a, int b, float length, float comp) {
            pA.push_back(a);
            pB.push_back(b);
            restLength.push_back(length);
            compliance.push_back(comp);
            lambda.push_back(0.0f); // Reset lambda
            count++;
        }

        void Clear() {
            pA.clear(); pB.clear(); restLength.clear();
            compliance.clear(); lambda.clear();
            count = 0;
        }
    };

    class ClothConstraints {
    public:
        ClothConstraints();
        ~ClothConstraints();

        // Register Constraints into specific caches
        void AddStructural(int pA, int pB, float restLen, float compliance);
        void AddShear(int pA, int pB, float restLen, float compliance);
        void AddBending(int pA, int pB, float restLen, float compliance);

        // Main Solver Entry Point
        void PreSolve(float dt);
        void Solve(ClothParticleData& particles, float dt);

        void ClearAll();

        inline size_t GetTotalConstraints() const {
            return m_Structural.count + m_Shear.count + m_Bending.count;
        }

    private:
        // Core Math Optimizer
        void SolveBatch(ConstraintBatch& batch, ClothParticleData& particles, float invDtSq);

    private:
        // Alag-alag batches taaki future me hum isko Multithreading (Task System) me daal sakein
        ConstraintBatch m_Structural;
        ConstraintBatch m_Shear;
        ConstraintBatch m_Bending;
    };

}