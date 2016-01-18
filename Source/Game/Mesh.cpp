#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include "GLTools.h"

namespace tankwars {
    Mesh::Mesh(const Vertex* vertices, size_t numVertices, const uint16_t* indices, size_t numIndices)
            : elementCount(static_cast<GLsizei>(numIndices)) {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * numIndices, indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), bufferOffset(sizeof(glm::vec3)));
        glBindVertexArray(0);
    }

    Mesh::Mesh(Mesh&& other) {
        *this = std::move(other);
    }

    Mesh::~Mesh() {
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &elementBuffer);
        glDeleteVertexArrays(1, &vertexArray);
    }

    Mesh& Mesh::operator=(Mesh&& other) {
        std::swap(vertexBuffer, other.vertexBuffer);
        std::swap(elementBuffer, other.elementBuffer);
        std::swap(vertexArray, other.vertexArray);
        elementCount = other.elementCount;
        return *this;
    }
	Mesh& Mesh::operator+=(Mesh&& other) {
		std::swap(vertexBuffer, other.vertexBuffer);
		std::swap(elementBuffer, other.elementBuffer);
		std::swap(vertexArray, other.vertexArray);
		elementCount = other.elementCount;
		return *this;
	}

    void Mesh::render() const {
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0);
    }

    glm::mat4 buildMatrixFromTransform(const Transform& transform) {
        glm::mat4 result(1.0f);
        result = glm::translate(result, transform.translation);
        result *= glm::mat4_cast(transform.rotation);
        result = glm::scale(result, transform.scale);
        return result;
    }
}