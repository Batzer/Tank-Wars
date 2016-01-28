#include "VoxelTerrain.h"

#include <algorithm>

#include "Image.h"

namespace tankwars {
    VoxelTerrain::VoxelTerrain(btDiscreteDynamicsWorld* dynamicsWorld,
        size_t numChunksX, size_t numChunksY, size_t numChunksZ,
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
            chunks.emplace_back(dynamicsWorld, x * chunkWidth, y * chunkHeight, z * chunkDepth,
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

    VoxelTerrain VoxelTerrain::fromHeightMap(const std::string& path, btDiscreteDynamicsWorld* dynamicsWorld,
            size_t chunkWidth, size_t chunkHeight, size_t chunkDepth, size_t invHeightScale) {
        Image heightMap(path);

        size_t maxHeight = 1;
        for (int i = 0; i < heightMap.getWidth() * heightMap.getHeight(); i++) {
            auto heightValue = heightMap.getImage()[i * heightMap.getNumChannels()];
            maxHeight = std::max(maxHeight, static_cast<size_t>(heightValue));
        }
        maxHeight = std::max(static_cast<size_t>(1), maxHeight / invHeightScale);

        auto numChunksX = heightMap.getWidth() / chunkWidth;
        if (heightMap.getWidth() % chunkWidth != 0) {
            numChunksX++;
        }

        auto numChunksY = maxHeight / chunkHeight;
        if (maxHeight % chunkHeight != 0) {
            numChunksY++;
        }

        auto numChunksZ = heightMap.getHeight() / chunkDepth;
        if (heightMap.getHeight() % chunkDepth != 0) {
            numChunksZ++;
        }

        VoxelTerrain terrain(dynamicsWorld, numChunksX, numChunksY, numChunksZ,
            chunkWidth, chunkHeight, chunkDepth);

        for (int z = 0; z < heightMap.getHeight(); z++)
        for (int x = 0; x < heightMap.getWidth(); x++) {
            auto index = (heightMap.getWidth() - x) + (heightMap.getHeight() - z) * heightMap.getWidth();
            auto heightValue = static_cast<int>(heightMap.getImage()[index * heightMap.getNumChannels()]);
            heightValue /= invHeightScale;

            for (int y = 0; y <= heightValue; y++) {
                terrain.setVoxel(x, y, z, VoxelType::Solid);
            }
        }

        terrain.updateMesh();
        return terrain;
    }
}