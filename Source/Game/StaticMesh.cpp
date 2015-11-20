#include "StaticMesh.h"
#include "GLTools.h"
#include <utility>

namespace gp {
    StaticMesh::StaticMesh(const VertexPosNormal* vertices, GLsizei vertexCount,
        const GLushort* indices, GLsizei indexCount)
        : _indexCount(indexCount)
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPosNormal) * vertexCount, vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexCount, indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosNormal), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosNormal), bufferOffset(sizeof(glm::vec3)));
        glBindVertexArray(0);
    }
    
    StaticMesh::StaticMesh(StaticMesh&& other) {
        *this = std::move(other);
    }

    StaticMesh::~StaticMesh() {
        glDeleteBuffers(1, &_vbo);
        glDeleteBuffers(1, &_ibo);
        glDeleteVertexArrays(1, &_vao);
    }

    StaticMesh& StaticMesh::operator=(StaticMesh&& other) {
        std::swap(_vbo, other._vbo);
        std::swap(_ibo, other._ibo);
        std::swap(_vao, other._vao);
        _indexCount = other._indexCount;
        return *this;
    }

    void StaticMesh::bindBuffers() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBindVertexArray(_vao);
    }

    void StaticMesh::render() const {
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0);
    }
}