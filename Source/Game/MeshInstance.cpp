#include "MeshInstance.h"

namespace tankwars {
    MeshInstance::MeshInstance(const Mesh& mesh, Material& material)
            : mesh(&mesh),
              material(&material) {
        // Do nothing
    }
}