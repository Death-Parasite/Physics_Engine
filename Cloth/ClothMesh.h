#pragma once

namespace Physics {
    class ClothSolver;

    // High-density Topology Grid Generator for 6k faces
    void GenerateClothGrid(ClothSolver* solver, int width, int height, float spacing, float totalMass);
}