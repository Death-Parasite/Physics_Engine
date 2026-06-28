#include "OBJExporter.h"
#include <fstream>
#include <iostream>

namespace Physics {
    void OBJExporter::ExportFrame(const std::string& filepath, const ClothParticleData& data) {
        std::ofstream file(filepath);
        if (!file.is_open()) return;

        file << "# NextGen Physics Engine Frame Dump\n";

        for (size_t i = 0; i < data.count; ++i) {
            const auto& p = data.positions[i];
            file << "v " << p.x << " " << p.y << " " << p.z << "\n";
        }

        for (size_t i = 0; i < data.triangles.size(); i += 3) {
            file << "f " << (data.triangles[i] + 1) << " " 
                         << (data.triangles[i+1] + 1) << " " 
                         << (data.triangles[i+2] + 1) << "\n";
        }

        file.close();
    }
}