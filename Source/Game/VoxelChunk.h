#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionShapes\btBvhTriangleMeshShape.h>
#include <BulletCollision\CollisionShapes\btTriangleMesh.h>
#include <GL/gl3w.h>

#include "Vertex.h"

namespace tankwars {
    enum class VoxelType {
        Empty,
        Solid
    };
    
    class VoxelChunk {
    public:
        VoxelChunk(btDiscreteDynamicsWorld* dynamicsWorld,
            size_t startX, size_t startY, size_t startZ, size_t width,
            size_t height, size_t depth, const VoxelType* voxels = nullptr);
        VoxelChunk(const VoxelChunk&) = delete;
        VoxelChunk(VoxelChunk&& other);
        ~VoxelChunk();

        VoxelChunk& operator=(const VoxelChunk&) = delete;
        VoxelChunk& operator=(VoxelChunk&& other);

        void setVoxel(size_t x, size_t y, size_t z, VoxelType voxel);
        VoxelType getVoxel(size_t x, size_t y, size_t z) const;

        void render() const;
        void updateMesh();

    private:
        void createCubeAt(size_t x, size_t y, size_t z, uint32_t faceMask);
        void createCollisionMesh();

        size_t startX, startY, startZ;
        size_t width, height, depth;
        std::vector<VoxelType> voxels;

        btDiscreteDynamicsWorld* dynamicsWorld;
        std::unique_ptr<btTriangleMesh> triangleMesh;
        std::unique_ptr<btBvhTriangleMeshShape> collisionMesh;
        std::unique_ptr<btCollisionObject> collisionObject;

        GLuint vertexArray;
        GLuint vertexArrayBuffer;
        GLuint elementArrayBuffer;

        bool isMeshDirty = true;
        std::vector<Vertex> vertexCache;
        std::vector<uint16_t> elementCache;
        size_t numVertices;
        size_t numElements;
    };
}