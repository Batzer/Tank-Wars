#include "Terrain.h"
#include <vector>
#include <iostream>
namespace tankwars {
	Terrain::Terrain(char* mapfileName,float maxHeight) 
		    : maxHeight(maxHeight), terrainMesh(createTerrainMesh(mapfileName)) {
		//Do nothing
	}
    Terrain::Terrain(const float* heightMap, size_t width, size_t height)
            : terrainMesh(createTerrainMesh(heightMap, width, height)) {
        // Do nothing
    }

    void Terrain::render() const {
        terrainMesh.render();
    }
	Mesh Terrain::createTerrainMesh(char* mapfileName) {
		readBMP(mapfileName,&width,&height);						// when added to constructer compiler says sth about it being a non-static member
		return createTerrainMesh(map, width, height);
	}
    Mesh Terrain::createTerrainMesh(const float* heightMap, size_t width, size_t height) {
        std::vector<glm::vec3> positions;
        positions.reserve(width * height);
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                auto heightValue = heightMap[x + y * width];
                positions.emplace_back(static_cast<float>(x), heightValue, static_cast<float>(y));
            }
        }

        std::vector<uint16_t> indices;
        for (size_t y = 0; y < height - 1; y++) {
            for (size_t x = 0; x < width - 1; x++) {
                auto baseIndex = static_cast<uint16_t>(x + y * width);
                indices.push_back(baseIndex);
                indices.push_back(baseIndex + static_cast<uint16_t>(width));
                indices.push_back(baseIndex + static_cast<uint16_t>(width) + 1);
                indices.push_back(baseIndex);
                indices.push_back(baseIndex + static_cast<uint16_t>(width) + 1);
                indices.push_back(baseIndex + 1);
            }
        }

        std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f));
        for (size_t i = 0; i < indices.size() / 3; i++) {
            auto p1 = positions[indices[i * 3]];
            auto p2 = positions[indices[i * 3 + 1]];
            auto p3 = positions[indices[i * 3 + 2]];
            auto n = glm::cross(p2 - p1, p3 - p1);

            normals[indices[i * 3]] += n;
            normals[indices[i * 3 + 1]] += n;
            normals[indices[i * 3 + 2]] += n;
        }

        for (auto& normal : normals) {
            normal = glm::normalize(normal);
        }

        std::vector<Vertex> vertices;
        vertices.reserve(width * height);
        for (size_t i = 0; i < positions.size(); i++) {
            vertices.push_back({positions[i], normals[i]});
        }

        return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
    }
	void Terrain::readBMP(char* filename,size_t* width,size_t* height) // maybe gotta change this a bit since it's copy-pasted from the internet except for fopen_s part
	{
		//int i;
		FILE* f;// = fopen(filename, "rb");
		fopen_s(&f, filename, "rb");
		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

												   // extract image height and width from header
		*width = *(int*)&info[18];
		*height = *(int*)&info[22];

		size_t size = 3 * *width * *height;
		unsigned char* data = new unsigned char[size];
		fread(data, sizeof(unsigned char), size, f);
		fclose(f);
		/*
		for (i = 0; i < size; i += 3)
		{
			unsigned char tmp = data[i];
			data[i] = data[i + 2];
			data[i + 2] = tmp;						// RGB instead of GRB
		}
		*/
		map = new float[size/3];								
		float scalingFactor = maxHeight / 255;
		for (size_t i = 0; i < size/3; i++) {												
			map[i] = scalingFactor-data[i*3]*scalingFactor;				//need to mirror this damn map to display the image right
			if ((i % *width) == (*width-1) ||(i % *width)==0||i>(*width* *width-*width)||i<*width) {
				map[i] = maxHeight+2;													// create borders
			}
		}

	}
	void Terrain::updateTerrain(glm::vec2 startingPoint, const float* newArea, size_t newAreaWidth, size_t newAreaHeight) { // is this function needed?
		for (size_t y = 0; y < newAreaHeight; y++) {
			for (size_t x = 0; x < newAreaWidth; x++) {
				//terrainMesh
			}
		}
	}
	void Terrain::explosionAt(glm::vec3 location,float radius){
		int size = int(radius) * 2+1;
		size_t startX=location.x-int(radius)+1;
		size_t startZ = location.z - int(radius) + 1;
		for (size_t x = startX; x < startX+size; x++) {
			for (size_t z = startZ; z < startZ+size; z++) {
				float distanceSquared = glm::pow(x - location.x, 2) + glm::pow(z - location.z, 2);
				if (glm::sqrt(distanceSquared)<=radius) {
					float coordY = location.y - glm::sqrt(glm::pow(radius, 2) - distanceSquared);
					//change the mesh according to the calculations
					if(map[x + z*width]>coordY)
						map[x + z*width] = coordY;
				}
			}
		}
		terrainMesh = createTerrainMesh(map, width, height);				// gotta be a more efficient way?!
	}
}