#pragma once

#include "StaticMesh.h"

namespace gp {
    // Creates a box with the origin as its center
    StaticMesh createBoxMesh(float width, float height, float depth);

    // Creates a x-z-plane
    // TODO: Make this create a grid
    StaticMesh createPlane(float width, float depth);
}