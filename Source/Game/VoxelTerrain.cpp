#include "VoxelTerrain.h"

#include <vector>
#include <cstring> // memcpy
#include "Image.h"

namespace {
    const tankwars::Vertex CubeVertices[] = {
        // Front
        { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1) },
        { glm::vec3( 0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1) },
        { glm::vec3( 0.5f,  0.5f, 0.5f), glm::vec3(0, 0, 1) },
        { glm::vec3(-0.5f,  0.5f, 0.5f), glm::vec3(0, 0, 1) },

        // Left
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0) },
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1, 0, 0) },
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1, 0, 0) },
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1, 0, 0) },

        // Right
        { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1, 0, 0) },
        { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0) },
        { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1, 0, 0) },
        { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1, 0, 0) },

        // Back
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1) },
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1) },
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1) },
        { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1) },

        // Top
        { glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(0, 1, 0) },
        { glm::vec3( 0.5f, 0.5f,  0.5f), glm::vec3(0, 1, 0) },
        { glm::vec3( 0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0) },
        { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0) },

        // Bottom
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0) },
        { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0) },
        { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0, -1, 0) },
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, -1, 0) }
    };

    const uint16_t CubeIndices[] = {
        0,  1,  2,  0,  2,  3,  // Front
        4,  5,  6,  4,  6,  7,  // Left
        8,  9,  10, 8,  10, 11, // Right
        12, 13, 14, 12, 14, 15, // Back
        16, 17, 18, 16, 18, 19, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };

    constexpr int LeftNeighbour = 1 << 0;
    constexpr int RightNeighbour = 1 << 1;
    constexpr int TopNeighbour = 1 << 2;
    constexpr int BottomNeighbour = 1 << 3;
    constexpr int FrontNeighbour = 1 << 4;
    constexpr int BackNeighbour = 1 << 5;
}

namespace tankwars {
    VoxelTerrain::VoxelTerrain(const bool* voxelGrid, size_t width, size_t height, size_t depth) {
        this->voxelGrid.reset(new bool[width * height * depth]);
        memcpy(this->voxelGrid.get(), voxelGrid, sizeof(bool) * width * height * depth);
        this->width = width;
        this->height = height;
        this->depth = depth;

        

        //updateMesh();
        createTerrainMesh(voxelGrid, width, height, depth);
    }

    VoxelTerrain::~VoxelTerrain() {
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &elementBuffer);
        glDeleteVertexArrays(1, &vertexArray);
    }

    void VoxelTerrain::render() const {
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
    }

    void VoxelTerrain::setVoxel(const glm::vec3& pos, bool active) {
        voxelGrid[pos.x + pos.y * width + pos.z * width * height] = active;
    }

    void VoxelTerrain::updateMesh() {
        createTerrainMesh(voxelGrid.get(), width, height, depth);
    }

    void VoxelTerrain::createTerrainMesh(const bool* voxelGrid, size_t width, size_t height, size_t depth) {
        auto get = [&](size_t x, size_t y, size_t z) -> bool {
            return voxelGrid[x + y * width + z * width * height];
        };
        
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        
        for (size_t z = 0; z < depth; z++)
        for (size_t y = 0; y < height; y++)
        for (size_t x = 0; x < width; x++) {
            if (get(x, y, z)) {
                int visiblityMask = 0;
            
                if (x > 0 && get(x - 1, y, z)) {
                    visiblityMask |= LeftNeighbour;
                }

                if (x < width - 1 && get(x + 1, y, z)) {
                    visiblityMask |= RightNeighbour;
                }

                if (y > 0 && get(x, y - 1, z)) {
                    visiblityMask |= BottomNeighbour;
                }

                if (y < height - 1 && get(x, y + 1, z)) {
                    visiblityMask |= TopNeighbour;
                }

                if (z > 0 && get(x, y, z - 1)) {
                    visiblityMask |= BackNeighbour;
                }

                if (z < depth - 1 && get(x, y, z + 1)) {
                    visiblityMask |= FrontNeighbour;
                }

                createCubeMesh({x, y, z}, visiblityMask, vertices, indices);
            }
        }

        if (vertexBuffer == 0) {
            glGenBuffers(1, &vertexBuffer);
            glGenBuffers(1, &elementBuffer);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 24 * width * height * depth, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 36 * width * height * depth, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());
        
        if (vertexArray == 0) {
            glGenVertexArrays(1, &vertexArray);
            glBindVertexArray(vertexArray);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(glm::vec3)));
            glBindVertexArray(0);
        }

        elementCount = static_cast<GLsizei>(indices.size());
    }

    void VoxelTerrain::createCubeMesh(const glm::vec3& pos, int visibilityMask, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        auto baseIndex = static_cast<uint32_t>(vertices.size());
        uint32_t index = 0;
        glm::vec3 offset = pos;

        // Front
        if ((visibilityMask & FrontNeighbour) == 0) {
            vertices.push_back({ CubeVertices[0].position + offset, CubeVertices[0].normal });
            vertices.push_back({ CubeVertices[1].position + offset, CubeVertices[1].normal });
            vertices.push_back({ CubeVertices[2].position + offset, CubeVertices[2].normal });
            vertices.push_back({ CubeVertices[3].position + offset, CubeVertices[3].normal });

            indices.push_back(baseIndex + index);
            indices.push_back(baseIndex + index + 1);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 0);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 3);
            index += 4;
        }

        // Left
        if ((visibilityMask & LeftNeighbour) == 0) {
            vertices.push_back({ CubeVertices[4].position + offset, CubeVertices[4].normal });
            vertices.push_back({ CubeVertices[5].position + offset, CubeVertices[5].normal });
            vertices.push_back({ CubeVertices[6].position + offset, CubeVertices[6].normal });
            vertices.push_back({ CubeVertices[7].position + offset, CubeVertices[7].normal });

            indices.push_back(baseIndex + index);
            indices.push_back(baseIndex + index + 1);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 0);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 3);
            index += 4;
        }

        // Right
        if ((visibilityMask & RightNeighbour) == 0) {
            vertices.push_back({ CubeVertices[8].position + offset, CubeVertices[8].normal });
            vertices.push_back({ CubeVertices[9].position + offset, CubeVertices[9].normal });
            vertices.push_back({ CubeVertices[10].position + offset, CubeVertices[10].normal });
            vertices.push_back({ CubeVertices[11].position + offset, CubeVertices[11].normal });

            indices.push_back(baseIndex + index);
            indices.push_back(baseIndex + index + 1);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 0);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 3);
            index += 4;
        }

        // Back
        if ((visibilityMask & BackNeighbour) == 0) {
            vertices.push_back({ CubeVertices[12].position + offset, CubeVertices[12].normal });
            vertices.push_back({ CubeVertices[13].position + offset, CubeVertices[13].normal });
            vertices.push_back({ CubeVertices[14].position + offset, CubeVertices[14].normal });
            vertices.push_back({ CubeVertices[15].position + offset, CubeVertices[15].normal });

            indices.push_back(baseIndex + index);
            indices.push_back(baseIndex + index + 1);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 0);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 3);
            index += 4;
        }

        // Top
        if ((visibilityMask & TopNeighbour) == 0) {
            vertices.push_back({ CubeVertices[16].position + offset, CubeVertices[16].normal });
            vertices.push_back({ CubeVertices[17].position + offset, CubeVertices[17].normal });
            vertices.push_back({ CubeVertices[18].position + offset, CubeVertices[18].normal });
            vertices.push_back({ CubeVertices[19].position + offset, CubeVertices[19].normal });

            indices.push_back(baseIndex + index);
            indices.push_back(baseIndex + index + 1);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 0);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 3);
            index += 4;
        }

        // Bottom
        if ((visibilityMask & BottomNeighbour) == 0) {
            vertices.push_back({ CubeVertices[20].position + offset, CubeVertices[20].normal });
            vertices.push_back({ CubeVertices[21].position + offset, CubeVertices[21].normal });
            vertices.push_back({ CubeVertices[22].position + offset, CubeVertices[22].normal });
            vertices.push_back({ CubeVertices[23].position + offset, CubeVertices[23].normal });

            indices.push_back(baseIndex + index);
            indices.push_back(baseIndex + index + 1);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 0);
            indices.push_back(baseIndex + index + 2);
            indices.push_back(baseIndex + index + 3);
            index += 4;
        }
    }
}