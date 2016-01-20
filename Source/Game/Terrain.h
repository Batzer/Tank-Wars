#pragma once

#include <cstddef>

#include "Mesh.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h>
#include <vector>
#include <string>

namespace tankwars {
    class Terrain {
    public:
		Terrain(const std::string& mapfileName, float maxHeight);
        Terrain(const float* heightMap, size_t width, size_t height);
        void render() const;
		void explosionAt(glm::vec3 location, float radius);
		float getHeightAt(int x, int z);
		//void explosionAt2(glm::vec3 location, float radius);

    private:
		std::vector<std::vector<std::vector<bool>>> voxelMap; // width, height, length 
		float maxHeight;
		float* map;				// is a variable needed or is it better to use a return value in "readBMP"
		size_t width,length;
		Mesh createTerrainMesh(const std::string& mapfileName);
        Mesh createTerrainMesh(const float* heightMap, size_t width, size_t height);
		void updateTerrain(glm::vec2 startingPoint, const float* newArea, size_t newAreaWidth, size_t newAreaHeight); // not implemented/ needed?
		void readBMP(const std::string& filename, size_t* width, size_t* height);		// move somewehere else later?!
        Mesh terrainMesh;
		btHeightfieldTerrainShape btTerrain;

		//Mesh Terrain2;
		//void readBMP2(char* filename, size_t* width, size_t* height);
		//void Terrain::createCubeVector();
		//Mesh marchingCubesAlgorithm(glm::vec3 firstVoxel, glm::vec3 lastVoxel);
		//void marchingCube(std::vector<Vertex>* vertices, int x, int y, int z);

	};
}