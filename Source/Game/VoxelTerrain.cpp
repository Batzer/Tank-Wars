#include "VoxelTerrain.h"

namespace tankwars {
    VoxelTerrain::VoxelTerrain(size_t numChunksX, size_t numChunksY, size_t numChunksZ,
        size_t chunkWidth, size_t chunkHeight, size_t chunkDepth)
            : numChunksX(numChunksX),
              numChunksY(numChunksY),
              numChunksZ(numChunksZ),
              chunkWidth(chunkWidth),
              chunkHeight(chunkHeight),
              chunkDepth(chunkDepth) {
        chunks.reserve(numChunksX * numChunksY * numChunksZ);
        for (size_t z = 0; z < numChunksZ; z++)
        for (size_t y = 0; y < numChunksY; y++)
        for (size_t x = 0; x < numChunksX; x++) {
            chunks.emplace_back(x * chunkWidth, y * chunkHeight, z * chunkDepth,
                chunkWidth, chunkHeight, chunkDepth);
        }
    }

    void VoxelTerrain::setVoxel(size_t x, size_t y, size_t z, VoxelType voxel) {
        assert(x < chunkWidth * numChunksX);
        assert(y < chunkHeight * numChunksY);
        assert(z < chunkDepth * numChunksZ);

        auto chunkIndex = (x / chunkWidth) + (y / chunkHeight) * numChunksX
            + (z / chunkDepth) * numChunksX * numChunksY;
        chunks[chunkIndex].setVoxel(x % chunkWidth, y % chunkHeight, z % chunkDepth, voxel);
    }

    VoxelType VoxelTerrain::getVoxel(size_t x, size_t y, size_t z) const {
        assert(x < chunkWidth * numChunksX);
        assert(y < chunkHeight * numChunksY);
        assert(z < chunkDepth * numChunksZ);

        auto chunkIndex = (x / chunkWidth) + (y / chunkHeight) * numChunksX
            + (z / chunkDepth) * numChunksX * numChunksY;
        return chunks[chunkIndex].getVoxel(x % chunkWidth, y % chunkHeight, z % chunkDepth);
    }

    size_t VoxelTerrain::getWidth() const {
        return chunkWidth * numChunksX;
    }

    size_t VoxelTerrain::getHeight() const {
        return chunkHeight * numChunksY;
    }

    size_t VoxelTerrain::getDepth() const {
        return chunkDepth * numChunksZ;
    }

    void VoxelTerrain::render() const {
        for (const auto& chunk : chunks) {
            chunk.render();
        }
    }

    void VoxelTerrain::updateMesh() {
        for (auto& chunk : chunks) {
            chunk.updateMesh();
        }
    }
}