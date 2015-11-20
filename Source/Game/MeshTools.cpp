#include "MeshTools.h"
#include <vector>

namespace gp {
    StaticMesh createBoxMesh(float width, float height, float depth) {
        const auto hw = width * 0.5f;
        const auto hh = height * 0.5f;
        const auto hd = depth * 0.5f;
        
        const VertexPosNormal vertices[] = {
            // Front
            { glm::vec3(-hw, -hh, hd), glm::vec3(0, 0, 1) },
            { glm::vec3( hw, -hh, hd), glm::vec3(0, 0, 1) },
            { glm::vec3( hw,  hh, hd), glm::vec3(0, 0, 1) },
            { glm::vec3(-hw,  hh, hd), glm::vec3(0, 0, 1) },

            // Left
            { glm::vec3(-hw, -hh, -hd), glm::vec3(-1, 0, 0) },
            { glm::vec3(-hw, -hh,  hd), glm::vec3(-1, 0, 0) },
            { glm::vec3(-hw,  hh,  hd), glm::vec3(-1, 0, 0) },
            { glm::vec3(-hw,  hh, -hd), glm::vec3(-1, 0, 0) },

            // Right
            { glm::vec3(hw, -hh,  hd), glm::vec3(1, 0, 0) },
            { glm::vec3(hw, -hh, -hd), glm::vec3(1, 0, 0) },
            { glm::vec3(hw,  hh, -hd), glm::vec3(1, 0, 0) },
            { glm::vec3(hw,  hh,  hd), glm::vec3(1, 0, 0) },

            // Back
            { glm::vec3( hw, -hh, -hd), glm::vec3(0, 0, -1) },
            { glm::vec3(-hw, -hh, -hd), glm::vec3(0, 0, -1) },
            { glm::vec3(-hw,  hh, -hd), glm::vec3(0, 0, -1) },
            { glm::vec3( hw,  hh, -hd), glm::vec3(0, 0, -1) },

            // Top
            { glm::vec3(-hw, hh,  hd), glm::vec3(0, 1, 0) },
            { glm::vec3( hw, hh,  hd), glm::vec3(0, 1, 0) },
            { glm::vec3( hw, hh, -hd), glm::vec3(0, 1, 0) },
            { glm::vec3(-hw, hh, -hd), glm::vec3(0, 1, 0) },

            // Bottom
            { glm::vec3(-hw, -hh, -hd), glm::vec3(0, -1, 0) },
            { glm::vec3( hw, -hh, -hd), glm::vec3(0, -1, 0) },
            { glm::vec3( hw, -hh,  hd), glm::vec3(0, -1, 0) },
            { glm::vec3(-hw, -hh,  hd), glm::vec3(0, -1, 0) }
        };

        const GLushort indices[] = {
            0,  1,  2,  0,  2,  3,  // Front
            4,  5,  6,  4,  6,  7,  // Left
            8,  9,  10, 8,  10, 11, // Right
            12, 13, 14, 12, 14, 15, // Back
            16, 17, 18, 16, 18, 19, // Top
            20, 21, 22, 20, 22, 23  // Bottom
        };

        return StaticMesh(vertices, 24, indices, 36);
    }

    StaticMesh createPlane(float width, float depth) {
        const auto hw = width * 0.5f;
        const auto hd = depth * 0.5f;

        const VertexPosNormal vertices[] = {
            { glm::vec3(-hw, 0,  hd), glm::vec3(0, 1, 0) },
            { glm::vec3( hw, 0,  hd), glm::vec3(0, 1, 0) },
            { glm::vec3( hw, 0, -hd), glm::vec3(0, 1, 0) },
            { glm::vec3(-hw, 0, -hd), glm::vec3(0, 1, 0) }
        };

        const GLushort indices[] = {
            0,  1,  2,  0,  2,  3
        };

        return StaticMesh(vertices, 4, indices, 6);
    }
}