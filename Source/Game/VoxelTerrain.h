#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Mesh.h"

namespace tankwars {
    class VoxelTerrain {
    public:
        VoxelTerrain(const bool* voxelGrid, size_t width, size_t height, size_t depth);
        ~VoxelTerrain();

        void render() const;

        void setVoxel(const glm::vec3& pos, bool active);
        void updateMesh();


    private:
        void createTerrainMesh(const bool* voxelGrid, size_t width, size_t height, size_t depth);
        void createCubeMesh(const glm::vec3& pos, int visibilityMask, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint elementBuffer = 0;
        GLsizei elementCount;
        std::unique_ptr<bool[]> voxelGrid;
        size_t width, height, depth;
    };
}