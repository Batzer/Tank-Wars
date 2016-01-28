#pragma once

#include <vector>
#include <string>

#include <btBulletDynamicsCommon.h>

#include "VoxelChunk.h"

namespace tankwars {
    class VoxelTerrain {
    public:
        VoxelTerrain(btDiscreteDynamicsWorld* dynamicsWorld,
            size_t numChunksX, size_t numChunksY, size_t numChunksZ,
            size_t chunkWidth, size_t chunkHeight, size_t chunkDepth);

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
        size_t numChunksX, numChunksY, numChunksZ;
        size_t chunkWidth, chunkHeight, chunkDepth;
        std::vector<VoxelChunk> chunks;
    };
}