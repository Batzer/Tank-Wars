#include "VoxelChunk.h"

#include <cassert>
#include <cstring> // memcpy
#include <utility>
#include <algorithm>

#include "GLTools.h"

namespace {
    using namespace tankwars;

    const glm::vec3 CubePositions[] = {
        // Front
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3( 0.5f, -0.5f, 0.5f),
        glm::vec3( 0.5f,  0.5f, 0.5f),
        glm::vec3(-0.5f,  0.5f, 0.5f),

        // Left
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f),

        // Right
        glm::vec3(0.5f, -0.5f,  0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f,  0.5f, -0.5f),
        glm::vec3(0.5f,  0.5f,  0.5f),

        // Back
        glm::vec3( 0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f),
        glm::vec3( 0.5f,  0.5f, -0.5f),

        // Top
        glm::vec3(-0.5f, 0.5f,  0.5f),
        glm::vec3( 0.5f, 0.5f,  0.5f),
        glm::vec3( 0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),

        // Bottom
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f, -0.5f,  0.5f),
        glm::vec3(-0.5f, -0.5f,  0.5f)
    };

    const glm::vec3 CubeNormals[] = {
        glm::vec3(0, 0, 1),
        glm::vec3(-1, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 1, 0),
        glm::vec3(0, -1, 0)
    };

    constexpr uint32_t FaceFront = 1;
    constexpr uint32_t FaceLeft = 2;
    constexpr uint32_t FaceRight = 4;
    constexpr uint32_t FaceBack = 8;
    constexpr uint32_t FaceTop = 16;
    constexpr uint32_t FaceBottom = 32;
}

namespace tankwars {
    VoxelChunk::VoxelChunk(size_t startX, size_t startY, size_t startZ, size_t width,
        size_t height, size_t depth, const VoxelType* voxels)
            : startX(startX),
              startY(startY),
              startZ(startZ),
              width(width),
              height(height),
              depth(depth),
              voxels(width * height * depth, VoxelType::Empty) {
        if (voxels) {
            memcpy(this->voxels.data(), voxels, width * height * depth * sizeof(VoxelType));
        }

        glGenBuffers(1, &vertexArrayBuffer);
        glGenBuffers(1, &elementArrayBuffer);
        glGenVertexArrays(1, &vertexArray);

        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), bufferOffset(sizeof(glm::vec3)));
        glBindVertexArray(0);
        
        vertexCache.resize(width * height * depth * 24);
        elementCache.resize(width * height * depth * 36);
		//chunckMesh = new btBvhTriangleMeshShape(nullptr, false); // replace nullptr with new btTriangleIndexVertexArray()
        updateMesh();
    }

    VoxelChunk::VoxelChunk(VoxelChunk&& other) {
        *this = std::move(other);
    }

    VoxelChunk::~VoxelChunk() {
        glDeleteBuffers(1, &vertexArrayBuffer);
        glDeleteBuffers(1, &elementArrayBuffer);
        glDeleteVertexArrays(1, &vertexArray);
    }

    VoxelChunk& VoxelChunk::operator=(VoxelChunk&& other) {
        startX = other.startX;
        startY = other.startY;
        startZ = other.startZ;
        width = other.width;
        height = other.height;
        depth = other.depth;
        voxels = std::move(other.voxels);
        std::swap(vertexArray, other.vertexArray);
        std::swap(vertexArrayBuffer, other.vertexArrayBuffer);
        std::swap(elementArrayBuffer, other.elementArrayBuffer);
        isMeshDirty = other.isMeshDirty;
        vertexCache = std::move(other.vertexCache);
        elementCache = std::move(other.elementCache);
        numVertices = other.numVertices;
        numElements = other.numElements;

		chunckMesh = other.chunckMesh;
        return *this;
    }

    void VoxelChunk::setVoxel(size_t x, size_t y, size_t z, VoxelType voxel) {
        assert(x < width && y < height && z < depth);

        auto index = x + y * width + z * width * height;
        isMeshDirty = (voxels[index] != voxel);
        voxels[index] = voxel;
    }

    VoxelType VoxelChunk::getVoxel(size_t x, size_t y, size_t z) const {
        assert(x < width && y < height && z < depth);
        return voxels[x + y * width + z * width * height];
    }

    void VoxelChunk::render() const {
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numElements), GL_UNSIGNED_SHORT, 0);
    }

    void VoxelChunk::updateMesh() {
        if (!isMeshDirty) {
            return;
        }

        numVertices = 0;
        numElements = 0;

        for (size_t z = 0; z < depth; z++)
        for (size_t y = 0; y < height; y++)
        for (size_t x = 0; x < width; x++) {
            if (getVoxel(x, y, z) == VoxelType::Empty) {
                continue;
            }

            uint32_t faceMask = 0;
            if (x > 0) {
                faceMask |= (getVoxel(x - 1, y, z) == VoxelType::Empty) ? FaceLeft : 0;
            }
            else {
                faceMask |= FaceLeft;
            }

            if (y > 0) {
                faceMask |= (getVoxel(x, y - 1, z) == VoxelType::Empty) ? FaceBottom : 0;
            }
            else {
                faceMask |= FaceBottom;
            }

            if (z > 0) {
                faceMask |= (getVoxel(x, y, z - 1) == VoxelType::Empty) ? FaceFront : 0;
            }
            else {
                faceMask |= FaceFront;
            }

            if (x < width - 1) {
                faceMask |= (getVoxel(x + 1, y, z) == VoxelType::Empty) ? FaceRight : 0;
            }
            else {
                faceMask |= FaceRight;
            }

            if (y < height - 1) {
                faceMask |= (getVoxel(x, y + 1, z) == VoxelType::Empty) ? FaceTop : 0;
            }
            else {
                faceMask |= FaceTop;
            }

            if (z < depth - 1) {
                faceMask |= (getVoxel(x, y, z + 1) == VoxelType::Empty) ? FaceBack : 0;
            }
            else {
                faceMask |= FaceBack;
            }

            createCubeAt(x, y, z, faceMask);
        }

        auto vertexArrayBufferSize = static_cast<GLsizei>(width * height * depth * 24 * sizeof(Vertex));
        auto elementArrayBufferSize = static_cast<GLsizei>(width * height * depth * 36 * sizeof(uint16_t));

        glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertexArrayBufferSize, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vertex), vertexCache.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferSize, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numElements * sizeof(uint16_t), elementCache.data());

        isMeshDirty = false;
    }

    void VoxelChunk::createCubeAt(size_t x, size_t y, size_t z, uint32_t faceMask) {
        static const uint32_t FaceBitmasks[] = { FaceFront, FaceLeft, FaceRight, FaceBack, FaceTop, FaceBottom };    
        auto baseElement = static_cast<uint16_t>(numVertices);
        glm::vec3 offset(startX + x, startY + y, -static_cast<float>(startZ + z));

        for (int i = 0; i < 6; i++) {
            if ((FaceBitmasks[i] & faceMask) == 0) {
                continue;
            }

            vertexCache[numVertices] = {CubePositions[i * 4] + offset, CubeNormals[i]};
            vertexCache[numVertices + 1] = {CubePositions[i * 4 + 1] + offset, CubeNormals[i]};
            vertexCache[numVertices + 2] = {CubePositions[i * 4 + 2] + offset, CubeNormals[i]};
            vertexCache[numVertices + 3] = {CubePositions[i * 4 + 3] + offset, CubeNormals[i]};

            elementCache[numElements] = baseElement;
            elementCache[numElements + 1] = baseElement + 1;
            elementCache[numElements + 2] = baseElement + 2;
            elementCache[numElements + 3] = baseElement;
            elementCache[numElements + 4] = baseElement + 2;
            elementCache[numElements + 5] = baseElement + 3;

            baseElement += 4;
            numVertices += 4;
            numElements += 6;
        }
    }
}