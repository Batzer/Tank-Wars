#pragma once

#include <cstddef>
#include <cstdint>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace tankwars {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct Material {
        glm::vec3 diffuseColor = { 1, 1, 1 };
        glm::vec3 specularColor = { 1, 1, 1 };
        float specularExponent = 64.0f;
    };
    
    class Mesh {
    public:
        Mesh(const Vertex* vertices, size_t numVertices, const uint16_t* indices, size_t numIndices);
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

    struct Transform {
        glm::vec3 translation = {0, 0, 0};
        glm::quat rotation = {1, 0, 0, 0};
        glm::vec3 scale = {1, 1, 1};
    };

    struct MeshInstance {
        Mesh* mesh;
        Material* material;
        Transform transform;
    };

    glm::mat4 buildMatrixFromTransform(const Transform& transform);
}