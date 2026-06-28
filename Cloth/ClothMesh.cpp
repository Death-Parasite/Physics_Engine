#include "ClothMesh.h"
#include "ClothSolver.h"
#include "../Math/VectorMath.h"

namespace Physics {

    void GenerateClothGrid(ClothSolver* solver, int width, int height, float spacing, float totalMass) {
        float particleMass = totalMass / (width * height);

        // Center calculation taaki kapda exact origin (0,0,0) ke upar ghire
        float startX = -(width * spacing) * 0.5f;
        float startZ = -(height * spacing) * 0.5f;

        // 1. Generate Particles in X-Z Plane (Flat Bedsheet in Sky)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vec3 pos(startX + (x * spacing), 2.5f, startZ + (y * spacing));
                solver->AddParticleToMesh(pos, particleMass);
            }
        }

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int current = y * width + x;

                if (x < width - 1) 
                    solver->AddConstraint(current, current + 1, spacing, 1.0f, ConstraintType::Distance);
                if (y < height - 1) 
                    solver->AddConstraint(current, current + width, spacing, 1.0f, ConstraintType::Distance);

                float diagLen = spacing * 1.414213f; 
                if (x < width - 1 && y < height - 1) {
                    solver->AddConstraint(current, current + width + 1, diagLen, 0.8f, ConstraintType::Distance);
                    solver->AddConstraint(current + 1, current + width, diagLen, 0.8f, ConstraintType::Distance);
                }
            }
        }

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