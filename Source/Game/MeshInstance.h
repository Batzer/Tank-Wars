#pragma once

#include <glm/glm.hpp>

namespace gp {
    class StaticMesh;
    struct Material;

    class MeshInstance {
    public:
        MeshInstance(const StaticMesh* mesh, const Material* material);

        const StaticMesh* getMesh() const;
        const Material* getMaterial() const;

        void setModelMatrix(const glm::mat4& matrix);
        const glm::mat4& getModelMatrix() const;
        const glm::mat4& getInvTrModelMatrix() const;

    private:
        const StaticMesh* _mesh;
        const Material* _material;
        glm::mat4 _modelMatrix = glm::mat4(1);
        glm::mat4 _invTrModelMatrix = glm::mat4(1);
    };
}