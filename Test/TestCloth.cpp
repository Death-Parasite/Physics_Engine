#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include "../Cloth/ClothSolver.h"
#include "../Core/Utils/OBJExporter.h"

using namespace Physics;
namespace fs = std::filesystem;

int main() {
    std::cout << "=============================================" << std::endl;
    std::cout << "   NEXT-GEN 3D CLOTH SIMULATION BAKER        " << std::endl;
    std::cout << "=============================================" << std::endl;

    std::string outDir = "cloth_frames_3d";
    fs::create_directories(outDir);

    ClothSolver solver;
    solver.InitializeGridMesh(10, 10, 0.2f, 2.0f);

    solver.PinParticle(0); 
    solver.PinParticle(9); 

    Vec3 gravity(0.0f, -9.81f, 0.0f);
    float dt = 0.016f; 

    std::cout << ">> Baking 60 frames of 3D Mesh to folder: '" << outDir << "'..." << std::endl;

    for (int frame = 1; frame <= 60; ++frame) {
        solver.StepSimulation(dt, gravity, 8); 

        std::ostringstream ss;
        ss << outDir << "/frame_" << std::setw(3) << std::setfill('0') << frame << ".obj";

        OBJExporter::ExportFrame(ss.str(), solver.GetParticleData());

        if (frame % 15 == 0) {
            std::cout << "   [✓] Baked Frame " << frame << " / 60" << std::endl;
        }
    }

    std::cout << "\n>> SUCCESS! Poore 60 3D Models disk par bake ho chuke hain!" << std::endl;
    return 0;
}