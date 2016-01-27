#pragma once

#include <glm/glm.hpp>

namespace tankwars {
    class Mesh;
    struct Material;

    struct MeshInstance {
        MeshInstance() = default;
        MeshInstance(const Mesh& mesh, Material& material);
        
        const Mesh* mesh = nullptr;
        Material* material = nullptr;
        glm::mat4 modelMatrix{ 1.0f };
    };
}