#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <glm/glm.hpp>

#include "Mesh.h"

namespace tankwars {
    struct WavefrontModel {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        std::vector<uint16_t> triangles;
    };
    
    WavefrontModel readWavefrontFromFile(const std::string& path, bool revertWindings = false);
    Mesh createMeshFromWavefront(const WavefrontModel& model);
}