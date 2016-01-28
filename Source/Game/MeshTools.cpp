#include "MeshTools.h"
#include <vector>

namespace tankwars {
    Mesh createBoxMesh(float width, float height, float depth) {
        const auto hw = width * 0.5f;
        const auto hh = height * 0.5f;
        const auto hd = depth * 0.5f;
        
        const Vertex vertices[] = {
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

        const uint16_t indices[] = {
            0,  1,  2,  0,  2,  3,  // Front
            4,  5,  6,  4,  6,  7,  // Left
            8,  9,  10, 8,  10, 11, // Right
            12, 13, 14, 12, 14, 15, // Back
            16, 17, 18, 16, 18, 19, // Top
            20, 21, 22, 20, 22, 23  // Bottom
        };

        return Mesh(vertices, 24, indices, 36);
    }

    Mesh createPlaneMesh(float width, float depth) {
        const auto hw = width * 0.5f;
        const auto hd = depth * 0.5f;

        const Vertex vertices[] = {
            { glm::vec3(-hw, 0,  hd), glm::vec3(0, 1, 0) },
            { glm::vec3( hw, 0,  hd), glm::vec3(0, 1, 0) },
            { glm::vec3( hw, 0, -hd), glm::vec3(0, 1, 0) },
            { glm::vec3(-hw, 0, -hd), glm::vec3(0, 1, 0) }
        };

        const uint16_t indices[] = {
            0,  1,  2,  0,  2,  3
        };

        return Mesh(vertices, 4, indices, 6);
    }

    Mesh createSphereMesh(float radius, size_t numStacks, size_t numSlices) {
        std::vector<Vertex> vertices;
        vertices.push_back({ glm::vec3(0, radius, 0), glm::vec3(0, 1, 0) });

        for (size_t y = 1; y < numStacks; y++) {
            auto theta = y * glm::pi<float>() / numStacks;

            for (size_t x = 0; x < numSlices; x++) {
                auto phi = 2 * x * glm::pi<float>() / numSlices;

                glm::vec3 position(
                    cos(phi) * sin(theta) * radius,
                    cos(theta) * radius,
                    sin(phi) * sin(theta) * radius);
                glm::vec3 normal = glm::normalize(position);

                vertices.push_back({ position, normal });
            }
        }

        vertices.push_back({ glm::vec3(0, -radius, 0), glm::vec3(0, -1, 0) });

        std::vector<uint16_t> indices;
        for (uint16_t i = 0; i < numSlices; i++) {
            indices.push_back(0); // North pole
            indices.push_back(1 + (i + 1) % numSlices);
            indices.push_back(1 + i);
            
        }

        for (size_t y = 0; y < numStacks - 2; y++) {
            for (size_t x = 0; x < numSlices; x++) {
                indices.push_back(static_cast<uint16_t>(1 + y * numSlices + x));
                indices.push_back(static_cast<uint16_t>(1 + (y + 1) * numSlices + (x + 1) % numSlices));
                indices.push_back(static_cast<uint16_t>(1 + (y + 1) * numSlices + x));
                

                indices.push_back(static_cast<uint16_t>(1 + y * numSlices + x));
                indices.push_back(static_cast<uint16_t>(1 + y * numSlices + (x + 1) % numSlices));
                indices.push_back(static_cast<uint16_t>(1 + (y + 1) * numSlices + (x + 1) % numSlices));
                
            }
        }

        auto southPoleIndex = static_cast<uint16_t>(vertices.size() - 1);
        auto baseIndex = static_cast<uint16_t>(southPoleIndex - numSlices);
        for (uint16_t i = 0; i < numSlices; i++) {
            indices.push_back(baseIndex + i);
            indices.push_back(baseIndex + (i + 1) % numSlices);
            indices.push_back(southPoleIndex);
            
        }

        return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
    }
}