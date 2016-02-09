#pragma once

#include <cstddef>
#include "Mesh.h"

namespace tankwars {
    // Creates a box with the origin as its center
    Mesh createBoxMesh(float width, float height, float depth);

    // Creates a x-z-plane
    // TODO: Make this create a grid
    Mesh createPlaneMesh(float width, float depth);

    // Creates a sphere with the origin as its center
    Mesh createSphereMesh(float radius, size_t numStacks, size_t numSlices);
}