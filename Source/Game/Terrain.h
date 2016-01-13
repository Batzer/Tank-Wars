#pragma once

#include <cstddef>
#include "Mesh.h"

namespace tankwars {
    class Terrain {
    public:
        Terrain(const float* heightMap, size_t width, size_t height);
        void render() const;

    private:
        Mesh createTerrainMesh(const float* heightMap, size_t width, size_t height);

        Mesh terrainMesh;
    };
}