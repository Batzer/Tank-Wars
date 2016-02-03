#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

namespace tankwars {
    struct GridCell {
        glm::vec3 positions[8];
        uint8_t values[8];
    };

    void polygonize(const GridCell& gridCell,
                    std::vector<glm::vec3>& outPositions,
                    std::vector<uint32_t>& outIndices);
}