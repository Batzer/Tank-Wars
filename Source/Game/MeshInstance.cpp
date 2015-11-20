#include "MeshInstance.h"
#include "StaticMesh.h"
#include "Material.h"

namespace gp {
    MeshInstance::MeshInstance(const StaticMesh* mesh, const Material* material)
        : _mesh(mesh)
        , _material(material)
    {
    }

    const StaticMesh* MeshInstance::getMesh() const {
        return _mesh;
    }

    const Material* MeshInstance::getMaterial() const {
        return _material;
    }

    void MeshInstance::setModelMatrix(const glm::mat4& matrix) {
        _modelMatrix = matrix;
        _invTrModelMatrix = glm::transpose(glm::inverse(matrix));
    }

    const glm::mat4& MeshInstance::getModelMatrix() const {
        return _modelMatrix;
    }

    const glm::mat4& MeshInstance::getInvTrModelMatrix() const {
        return _invTrModelMatrix;
    }
}