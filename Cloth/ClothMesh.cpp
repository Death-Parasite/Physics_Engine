#include "ClothMesh.h"
#include "ClothSolver.h"
#include "../Math/VectorMath.h"

namespace Physics {

    void GenerateClothGrid(ClothSolver* solver, int width, int height, float spacing, float totalMass) {
        float particleMass = totalMass / (width * height);
        float startX = -(width * spacing) * 0.5f;
        float startZ = -(height * spacing) * 0.5f;

        // 1. Generate Particles
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vec3 pos(startX + (x * spacing), 2.2f, startZ + (y * spacing));
                solver->AddParticleToMesh(pos, particleMass);
            }
        }

        // 2. Generate Batched Constraints (Tuned for 6,000 Faces Strength)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int current = y * width + x;

                // Structural Constraints (Extremely tight cotton compliance)
                if (x < width - 1) 
                    solver->AddStructuralConstraint(current, current + 1, spacing, 0.0000001f);
                if (y < height - 1) 
                    solver->AddStructuralConstraint(current, current + width, spacing, 0.0000001f);

                // Shear Constraints (Diagonal lock)
                float diagLen = spacing * 1.414213f; 
                if (x < width - 1 && y < height - 1) {
                    solver->AddShearConstraint(current, current + width + 1, diagLen, 0.00005f);
                    solver->AddShearConstraint(current + 1, current + width, diagLen, 0.00005f);
                }

                // Long-range AAA Bending Constraints (Skip-2 structural nodes)
                if (x < width - 2)
                    solver->AddBendingConstraint(current, current + 2, spacing * 2.0f, 0.002f);
                if (y < height - 2)
                    solver->AddBendingConstraint(current, current + (width * 2), spacing * 2.0f, 0.002f);
            }
        }

        // 3. Generate Triangles Skin
        for (int y = 0; y < height - 1; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                int topLeft     = y * width + x;
                int topRight    = topLeft + 1;
                int bottomLeft  = (y + 1) * width + x;
                int bottomRight = bottomLeft + 1;

                solver->AddTriangle(topLeft, bottomLeft, topRight);
                solver->AddTriangle(topRight, bottomLeft, bottomRight);
            }
        }
    }
}