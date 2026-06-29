#include <iostream>
#include "../Cloth/ClothSolver.h"

using namespace Physics;

int main() {
    std::cout << "=== TIER 1000-FACES AUTOMATED STABILITY TEST ===" << std::endl;

    ClothSolver solver;
    solver.InitializeGridMesh(24, 24, 0.08f, 1.5f); // 1,058 Faces
    
    // --- YAHAN FIX HUA: SetSphereCollider ab AddSphereCollider ban chuka hai ---
    solver.AddSphereCollider(Vec3(0, 0.5f, 0), 0.7f);

    Vec3 gravity(0.0f, -9.81f, 0.0f);
    Vec3 wind(3.0f, 0.0f, 1.0f);

    std::cout << "Simulating 120 Frames of high-density collision..." << std::endl;

    for (int f = 1; f <= 120; ++f) {
        solver.StepSimulation(0.016f, gravity, wind, 10);
    }

    float finalY = solver.GetParticleData().positions[280].y;
    std::cout << "Center Particle Y after drape: " << finalY << std::endl;

    if (finalY > 0.45f && finalY < 1.3f) {
        std::cout << ">> 1K MILESTONE PASSED: Mesh rested stably on sphere!" << std::endl;
        return 0;
    } else {
        std::cout << ">> TEST FAILED: Explosion or clipping detected!" << std::endl;
        return 1;
    }
}