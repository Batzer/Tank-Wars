#include "Terrain.h"
#include <vector>

namespace tankwars {
    Terrain::Terrain(const float* heightMap, size_t width, size_t height)
            : terrainMesh(createTerrainMesh(heightMap, width, height)) {
        // Do nothing
    }

    void Terrain::render() const {
        terrainMesh.render();
    }

    Mesh Terrain::createTerrainMesh(const float* heightMap, size_t width, size_t height) {
        std::vector<glm::vec3> positions;
        positions.reserve(width * height);
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                auto heightValue = heightMap[x + y * width];
                positions.emplace_back(static_cast<float>(x), heightValue, static_cast<float>(y));
            }
        }

        std::vector<uint16_t> indices;
        for (size_t y = 0; y < height - 1; y++) {
            for (size_t x = 0; x < width - 1; x++) {
                auto baseIndex = static_cast<uint16_t>(x + y * width);
                indices.push_back(baseIndex);
                indices.push_back(baseIndex + static_cast<uint16_t>(width));
                indices.push_back(baseIndex + static_cast<uint16_t>(width) + 1);
                indices.push_back(baseIndex);
                indices.push_back(baseIndex + static_cast<uint16_t>(width) + 1);
                indices.push_back(baseIndex + 1);
            }
        }

        std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f));
        for (size_t i = 0; i < indices.size() / 3; i++) {
            auto p1 = positions[indices[i * 3]];
            auto p2 = positions[indices[i * 3 + 1]];
            auto p3 = positions[indices[i * 3 + 2]];
            auto n = glm::cross(p2 - p1, p3 - p1);

            normals[indices[i * 3]] += n;
            normals[indices[i * 3 + 1]] += n;
            normals[indices[i * 3 + 2]] += n;
        }

        for (auto& normal : normals) {
            normal = glm::normalize(normal);
        }

        std::vector<Vertex> vertices;
        vertices.reserve(width * height);
        for (size_t i = 0; i < positions.size(); i++) {
            vertices.push_back({positions[i], normals[i]});
        }

        return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
    }
}