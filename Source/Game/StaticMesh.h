#pragma once

#include "VertexTypes.h"
#include <GL/gl3w.h>

namespace gp {
    class StaticMesh {
    public:
        StaticMesh(const VertexPosNormal* vertices, GLsizei vertexCount,
            const GLushort* indices, GLsizei indexCount);
        StaticMesh(const StaticMesh&) = delete;
        StaticMesh(StaticMesh&& other);
        ~StaticMesh();

        StaticMesh& operator=(const StaticMesh&) = delete;
        StaticMesh& operator=(StaticMesh&& other);

        void bindBuffers() const;
        void render() const;

    private:
        GLuint _vbo;
        GLuint _ibo;
        GLuint _vao;
        GLsizei _indexCount;
    };
}