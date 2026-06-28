#pragma once
#include <string>
#include "../Types/PhysicsTypes.h"

namespace Physics {
    class OBJExporter {
    public:
        static void ExportFrame(const std::string& filepath, const ClothParticleData& data);
    };
}