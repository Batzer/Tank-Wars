#pragma once

#include <cstddef>
#include <cstdint>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "Vertex.h"

namespace tankwars {
    struct Material {
        glm::vec3 diffuseColor = { 1, 1, 1 };
        glm::vec3 specularColor = { 1, 1, 1 };
        float specularExponent = 64.0f;
    };
    
    class Mesh {
    public:
        Mesh(const Vertex* vertices, size_t numVertices, const uint32_t* indices, size_t numIndices);
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&& other);
        ~Mesh();

        Mesh& operator=(const Mesh&) = delete;
        Mesh& operator=(Mesh&& other);

        void render() const;

    private:
        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint elementBuffer;
        GLsizei elementCount;
    };
}
