#pragma once

#include <cstddef>
#include "Mesh.h"

namespace tankwars {
    class Terrain {
    public:
		Terrain(char* mapfileName,float maxHeight);
        Terrain(const float* heightMap, size_t width, size_t height);
        void render() const;
		void explosionAt(glm::vec3 location, float radius);
		float getHeightAt(int x, int z);
    private:
		float maxHeight;
		float* map;				// is a variable needed or is it better to use a return value in "readBMP"
		size_t width,height;
		Mesh createTerrainMesh(char* mapfileName);
        Mesh createTerrainMesh(const float* heightMap, size_t width, size_t height);
		void updateTerrain(glm::vec2 startingPoint, const float* newArea, size_t newAreaWidth, size_t newAreaHeight); // not implemented/ needed?
		void readBMP(char* filename, size_t* width, size_t* height);		// move somewehere else later?!
        Mesh terrainMesh;
    };
}