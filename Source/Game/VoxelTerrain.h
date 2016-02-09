#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>

#include <GL/gl3w.h>
#include <btBulletDynamicsCommon.h>

#include "Mesh.h"

namespace tankwars {
    enum class VoxelType : uint8_t {
        Empty = 0,
        Solid = 1
    };

    class VoxelTerrain {
    public:
        VoxelTerrain(btDiscreteDynamicsWorld* dynamicsWorld,
            size_t numChunksX, size_t numChunksY, size_t numChunksZ,
            size_t chunkWidth, size_t chunkHeight, size_t chunkDepth);
        VoxelTerrain(const VoxelTerrain&) = default;
        VoxelTerrain(VoxelTerrain&&) = default;
        ~VoxelTerrain();

        void setVoxel(size_t x, size_t y, size_t z, VoxelType voxel);
        VoxelType getVoxel(size_t x, size_t y, size_t z) const;
        size_t getWidth() const;
        size_t getHeight() const;
        size_t getDepth() const;

        void render() const;
        void updateMesh();

        static VoxelTerrain fromHeightMap(const std::string& path, btDiscreteDynamicsWorld* dynamicsWorld,
            size_t chunkWidth, size_t chunkHeight, size_t chunkDepth, size_t invHeightScale);
		
    private:
        size_t computeChunkIndex(size_t x, size_t y, size_t z) const;
        void updateChunk(size_t startX, size_t startY, size_t startZ);

        // Terrain
        size_t numChunksX, numChunksY, numChunksZ;
        size_t chunkWidth, chunkHeight, chunkDepth;
        std::vector<uint8_t> voxels;
        
        // Rendering
        std::vector<GLuint> chunkVertexArrays;
        std::vector<GLuint> chunkVertexArrayBuffers;
        std::vector<GLuint> chunkElementBuffers;
        std::vector<GLsizei> chunkElementCounts;
        std::vector<uint8_t> chunkDirtyStates;

        // Physics
        btDiscreteDynamicsWorld* dynamicsWorld;
        std::vector<std::unique_ptr<btTriangleMesh>> chunkTriangleMeshes;
        std::vector<std::unique_ptr<btBvhTriangleMeshShape>> chunkCollisionMeshes;
        std::vector<std::unique_ptr<btDefaultMotionState>> chunkMotionStates;
        std::vector<std::unique_ptr<btRigidBody>> chunkRigidBodies;
    };
}