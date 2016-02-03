#include "VoxelTerrain.h"

#include <algorithm>

#include "Image.h"
#include "GLTools.h"
#include "MarchingCubes.h"

namespace tankwars {
    VoxelTerrain::VoxelTerrain(btDiscreteDynamicsWorld* dynamicsWorld,
        size_t numChunksX, size_t numChunksY, size_t numChunksZ,
        size_t chunkWidth, size_t chunkHeight, size_t chunkDepth)
            : numChunksX(numChunksX),
              numChunksY(numChunksY),
              numChunksZ(numChunksZ),
              chunkWidth(chunkWidth),
              chunkHeight(chunkHeight),
              chunkDepth(chunkDepth),
              dynamicsWorld(dynamicsWorld) {
        auto numChunks = numChunksX * numChunksY * numChunksZ;
        auto numVoxels = numChunks * chunkWidth * chunkHeight * chunkDepth;
        voxels.resize(numVoxels, static_cast<uint8_t>(VoxelType::Empty));

        chunkVertexArrays.resize(numChunks);
        chunkVertexArrayBuffers.resize(numChunks);
        chunkElementBuffers.resize(numChunks);
        chunkElementCounts.resize(numChunks, 0);
        chunkDirtyStates.resize(numChunks, 0);

        glGenVertexArrays(static_cast<GLsizei>(numChunks), chunkVertexArrays.data());
        glGenBuffers(static_cast<GLsizei>(numChunks), chunkVertexArrayBuffers.data());
        glGenBuffers(static_cast<GLsizei>(numChunks), chunkElementBuffers.data());

        for (size_t i = 0; i < numChunks; i++) {
            glBindVertexArray(chunkVertexArrays[i]);
            glBindBuffer(GL_ARRAY_BUFFER, chunkVertexArrayBuffers[i]);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), bufferOffset(sizeof(glm::vec3)));
            glBindVertexArray(0);
        }

        chunkTriangleMeshes.resize(numChunks);
        chunkCollisionMeshes.resize(numChunks);
        chunkMotionStates.resize(numChunks);
        chunkRigidBodies.resize(numChunks);
    }

    VoxelTerrain::~VoxelTerrain() {
        glDeleteVertexArrays(static_cast<GLsizei>(chunkVertexArrays.size()), chunkVertexArrays.data());
        glDeleteBuffers(static_cast<GLsizei>(chunkVertexArrayBuffers.size()), chunkVertexArrayBuffers.data());
        glDeleteBuffers(static_cast<GLsizei>(chunkElementBuffers.size()), chunkElementBuffers.data());

        for (auto& body : chunkRigidBodies) {
            if (body) {
                dynamicsWorld->removeRigidBody(body.get());
            }
        }
    }

    void VoxelTerrain::setVoxel(size_t x, size_t y, size_t z, VoxelType voxel) {
        assert(x < chunkWidth * numChunksX);
        assert(y < chunkHeight * numChunksY);
        assert(z < chunkDepth * numChunksZ);

        auto index = x + y * getWidth() + z * getWidth() * getHeight();
        if (static_cast<VoxelType>(voxels[index]) != voxel) {
            auto chunkIndex = (x / chunkWidth) + (y / chunkHeight) * numChunksX
                + (z / chunkDepth) * numChunksX * numChunksY;
            voxels[index] = static_cast<uint8_t>(voxel);
            chunkDirtyStates[chunkIndex] = 1;
        }
    }

    VoxelType VoxelTerrain::getVoxel(size_t x, size_t y, size_t z) const {
        assert(x < chunkWidth * numChunksX);
        assert(y < chunkHeight * numChunksY);
        assert(z < chunkDepth * numChunksZ);

        auto index = x + y * getWidth() + z * getWidth() * getHeight();
        return static_cast<VoxelType>(voxels[index]);
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
        auto numChunks = numChunksX * numChunksY * numChunksZ;
        for (size_t i = 0; i < numChunks; i++) {
            if (chunkElementCounts[i] == 0) {
                continue;
            }

            glBindVertexArray(chunkVertexArrays[i]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkElementBuffers[i]);
            glDrawElements(GL_TRIANGLES, chunkElementCounts[i], GL_UNSIGNED_INT, 0);
        }
    }

    void VoxelTerrain::updateMesh() {
        for (size_t z = 0; z < numChunksZ; z++)
        for (size_t y = 0; y < numChunksY; y++)
        for (size_t x = 0; x < numChunksX; x++) {
            auto chunkIndex = x + y * numChunksX + z * numChunksX * numChunksY;
            if (chunkDirtyStates[chunkIndex]) {
                updateChunk(x, y, z);
                chunkDirtyStates[chunkIndex] = 0;
            }
        }
    }

    VoxelTerrain VoxelTerrain::fromHeightMap(const std::string& path, btDiscreteDynamicsWorld* dynamicsWorld,
            size_t chunkWidth, size_t chunkHeight, size_t chunkDepth, size_t invHeightScale) {
        Image heightMap(path);

        size_t maxHeight = 1;
        for (int i = 0; i < heightMap.getWidth() * heightMap.getHeight(); i++) {
            auto heightValue = heightMap.getImage()[i * heightMap.getNumChannels()];
            maxHeight = std::max(maxHeight, static_cast<size_t>(heightValue + 1));
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
            auto index = (heightMap.getWidth() - x - 1) + (heightMap.getHeight() - z - 1) * heightMap.getWidth();
            auto heightValue = static_cast<int>(heightMap.getImage()[index * heightMap.getNumChannels()]);
            heightValue /= invHeightScale;

            for (int y = 0; y <= heightValue; y++) {
                terrain.setVoxel(x, y, z, VoxelType::Solid);
            }
        }

        terrain.updateMesh();
        return terrain;
    }

    void VoxelTerrain::updateChunk(size_t startX, size_t startY, size_t startZ) {
        static std::vector<glm::vec3> posCache;
        static std::vector<glm::vec3> normalCache;
        static std::vector<Vertex> vertexCache;
        static std::vector<uint32_t> indexCache;

        posCache.clear();
        normalCache.clear();
        vertexCache.clear();
        indexCache.clear();

        GridCell gridCell;
        auto endX = std::min(startX * chunkWidth + chunkWidth, getWidth() - 1);
        auto endY = std::min(startY * chunkHeight + chunkHeight, getHeight() - 1);
        auto endZ = std::min(startZ * chunkDepth + chunkDepth, getDepth() - 1);

        for (size_t z = startZ * chunkDepth;  z < endZ; z++)
        for (size_t y = startY * chunkHeight; y < endY; y++)
        for (size_t x = startX * chunkWidth;  x < endX; x++) {
            gridCell.positions[0] = glm::vec3(x,     y,     z + 1);
            gridCell.positions[1] = glm::vec3(x + 1, y,     z + 1);
            gridCell.positions[2] = glm::vec3(x + 1, y,     z);
            gridCell.positions[3] = glm::vec3(x,     y,     z);
            gridCell.positions[4] = glm::vec3(x,     y + 1, z + 1);
            gridCell.positions[5] = glm::vec3(x + 1, y + 1, z + 1);
            gridCell.positions[6] = glm::vec3(x + 1, y + 1, z);
            gridCell.positions[7] = glm::vec3(x,     y + 1, z);

            gridCell.values[0] = static_cast<uint8_t>(getVoxel(x,     y,     z + 1));
            gridCell.values[1] = static_cast<uint8_t>(getVoxel(x + 1, y,     z + 1));
            gridCell.values[2] = static_cast<uint8_t>(getVoxel(x + 1, y,     z));
            gridCell.values[3] = static_cast<uint8_t>(getVoxel(x,     y,     z));
            gridCell.values[4] = static_cast<uint8_t>(getVoxel(x,     y + 1, z + 1));
            gridCell.values[5] = static_cast<uint8_t>(getVoxel(x + 1, y + 1, z + 1));
            gridCell.values[6] = static_cast<uint8_t>(getVoxel(x + 1, y + 1, z));
            gridCell.values[7] = static_cast<uint8_t>(getVoxel(x,     y + 1, z));

            polygonize(gridCell, posCache, indexCache);
        }

        auto chunkIndex = startX + startY * numChunksX + startZ * numChunksX * numChunksY;
        if (posCache.empty()) {
            chunkElementCounts[chunkIndex] = 0;

            if (chunkRigidBodies[chunkIndex]) {
                dynamicsWorld->removeRigidBody(chunkRigidBodies[chunkIndex].get());
                chunkRigidBodies[chunkIndex].reset();
            }

            return;
        }

        if (normalCache.size() < posCache.size()) {
            normalCache.resize(posCache.size(), glm::vec3(0.0f));
        }
        else {
            std::fill(normalCache.begin(), normalCache.end(), glm::vec3(0.0f));
        }

        for (size_t i = 0; i < indexCache.size() / 3; i++) {
            auto p1 = posCache[indexCache[i * 3]];
            auto p2 = posCache[indexCache[i * 3 + 1]];
            auto p3 = posCache[indexCache[i * 3 + 2]];
            auto n = glm::cross(p2 - p1, p3 - p1);

            normalCache[indexCache[i * 3]] += n;
            normalCache[indexCache[i * 3 + 1]] += n;
            normalCache[indexCache[i * 3 + 2]] += n;
        }

        for (auto& normal : normalCache) {
            normal = glm::normalize(normal);
        }

        if (vertexCache.capacity() < posCache.size()) {
            vertexCache.reserve(posCache.size());
        }

        for (size_t i = 0; i < posCache.size(); i++) {
            vertexCache.push_back({posCache[i], normalCache[i]});
        }

        auto vertexArrayBufferSize = static_cast<GLsizei>((chunkWidth + 1) * (chunkHeight + 1) * (chunkDepth + 1) * sizeof(Vertex));
        auto elementArrayBufferSize = static_cast<GLsizei>(chunkWidth * chunkHeight * chunkDepth * 36 * sizeof(uint32_t));

        glBindBuffer(GL_ARRAY_BUFFER, chunkVertexArrayBuffers[chunkIndex]);
        glBufferData(GL_ARRAY_BUFFER, vertexArrayBufferSize, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCache.size() * sizeof(Vertex), vertexCache.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkElementBuffers[chunkIndex]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferSize, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCache.size() * sizeof(uint32_t), indexCache.data());
        chunkElementCounts[chunkIndex] = indexCache.size();

        chunkTriangleMeshes[chunkIndex].reset(new btTriangleMesh);
        chunkTriangleMeshes[chunkIndex]->preallocateVertices(indexCache.size() / 3);
        chunkTriangleMeshes[chunkIndex]->preallocateIndices(indexCache.size());

        for (size_t i = 0; i < indexCache.size() / 3; i++) {
            auto index1 = indexCache[i * 3];
            auto index2 = indexCache[i * 3 + 1];
            auto index3 = indexCache[i * 3 + 2];

            const auto& pos1 = posCache[index1];
            const auto& pos2 = posCache[index2];
            const auto& pos3 = posCache[index3];

            chunkTriangleMeshes[chunkIndex]->addTriangle(
                btVector3(pos1.x, pos1.y, pos1.z),
                btVector3(pos2.x, pos2.y, pos2.z),
                btVector3(pos3.x, pos3.y, pos3.z));
        }

        if (chunkRigidBodies[chunkIndex]) {
            dynamicsWorld->removeRigidBody(chunkRigidBodies[chunkIndex].get());
        }
        
        chunkCollisionMeshes[chunkIndex].reset(new btBvhTriangleMeshShape(chunkTriangleMeshes[chunkIndex].get(), true));
        chunkMotionStates[chunkIndex].reset(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, chunkMotionStates[chunkIndex].get(),
                                                                   chunkCollisionMeshes[chunkIndex].get(), btVector3(0, 0, 0));
        chunkRigidBodies[chunkIndex].reset(new btRigidBody(groundRigidBodyCI));
        dynamicsWorld->addRigidBody(chunkRigidBodies[chunkIndex].get());
    }
}