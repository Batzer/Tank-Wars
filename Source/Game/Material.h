#pragma once

#include <glm/glm.hpp>

namespace gp {
    struct Material {
        glm::vec3 diffuse = { 1, 1, 1 };
        glm::vec3 specular = { 1, 1, 1 };
        float shininess = 64.0f;
    };
}