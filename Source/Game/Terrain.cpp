#include "Terrain.h"

#include <vector>
#include <iostream>
#include <cmath>

#include "Image.h"

namespace tankwars {
	Terrain::Terrain(const std::string& mapfileName, float maxHeight) 
		    : maxHeight(maxHeight), 
<<<<<<< HEAD
				terrainMesh(createTerrainMesh(mapfileName))/*,
				btTerrain(int(width),int(length),map,maxHeight,1,PHY_FLOAT,0)*/{
=======
			  terrainMesh(createTerrainMesh(mapfileName)),
			  btTerrain(static_cast<int>(width), static_cast<int>(length), map.data(), maxHeight, 1, PHY_FLOAT, 0) {
        // Do nothing
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
	}

    Terrain::Terrain(const float* heightMap, size_t width, size_t height)
<<<<<<< HEAD
            : terrainMesh(createTerrainMesh(heightMap, width, height))/*,
				btTerrain(int(width), int(length), map, maxHeight, 1, PHY_FLOAT, 0)*/ {
=======
            : terrainMesh(createTerrainMesh(heightMap, width, height)),
			  btTerrain(static_cast<int>(width), static_cast<int>(length), map.data(), maxHeight, 1, PHY_FLOAT, 0) {
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
        // Do nothing
    }

    void Terrain::render() const {
        terrainMesh.render();
    }

	Mesh Terrain::createTerrainMesh(const std::string& mapfileName) {
        readHeightMapFromFile(mapfileName, width, length);						// when added to constructer compiler says sth about it being a non-static member
		return createTerrainMesh(map.data(), width, length);
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

	void Terrain::readHeightMapFromFile(const std::string& filename, size_t& width, size_t& height) {
		Image heightMap(filename);

        width = heightMap.getWidth();
        height = heightMap.getHeight();
        map.resize(width * height);

        const auto pixels = heightMap.getImage();
        const auto scalingFactor = maxHeight / 255.0f;

        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                auto index = x + y * width;

                if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
                    map[index] = maxHeight + 2;
                }
                else {
                    auto heightValue = pixels[index * heightMap.getNumChannels()];
                    map[index] = scalingFactor - heightValue * scalingFactor;
                }
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

	void Terrain::explosionAt(const glm::vec3& location, float radius){
		auto size = static_cast<int>(radius) * 2 + 1;
		auto startX = location.x - static_cast<int>(radius) + 1;
		auto startZ = location.z - static_cast<int>(radius) + 1;

		for (size_t x = startX; x < startX + size; x++) {
			for (size_t z = startZ; z < startZ + size; z++) {
				float distanceSquared = pow(x - location.x, 2) + pow(z - location.z, 2);

				if (sqrt(distanceSquared) <= radius) {
					float coordY = location.y - sqrt(pow(radius, 2) - distanceSquared);

					//change the mesh according to the calculations
					if(map[x + z * width] > coordY) {
						map[x + z * width] = coordY;
                    }
				}
			}
		}

		terrainMesh = createTerrainMesh(map.data(), width, length);				// gotta be a more efficient way?!
	}

	float Terrain::getHeightAt(int x, int z) {
		return map[x + z * width];
	}
<<<<<<< HEAD
	/*void Terrain::readBMP2(char* filename, size_t* width, size_t*length) {
=======

    /*
	void Terrain::readBMP2(char* filename, size_t* width, size_t*length) {
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
		FILE* f;// = fopen(filename, "rb");
		fopen_s(&f, filename, "rb");
		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

												   // extract image height and width from header
		*width = *(int*)&info[18];
		*length = *(int*)&info[22];


		voxelMap.resize(*width);						//i don't like this being here:$
		for (size_t i = 0; i < *width; ++i) {
			voxelMap[i].resize(maxHeight);
			for (size_t j = 0; j < *length; ++j) {
				voxelMap[i][j].resize(int(*length));	//proportions?!??!?! 1:1 ?
			}
		}


		size_t size = 3 * *width * *length;
		unsigned char* data = new unsigned char[size];
		fread(data, sizeof(unsigned char), size, f);
		fclose(f);

		float scalingFactor = maxHeight / 255;
		for (size_t i = 0; i < size / 3; i++) {
			for (size_t j = 0; j < scalingFactor - data[i * 3] * scalingFactor; j++) {//is the scaling right?0.o
				voxelMap[i%*width][j][(int)i / (*width)]=1;								//ich glaube das geht so xD
			}
		}
	}
	void Terrain::explosionAt2(glm::vec3 location, float radius) {
		size_t i = location.x - radius < 0 ? 0 : location.x - radius;
		size_t j = location.z - radius < 0 ? 0 : location.z - radius;
		size_t h = location.y - radius < 0 ? 0 : location.y - radius;
		
		size_t i_max = location.x + radius > width ? width : location.x + radius;
		size_t j_max = location.z + radius > length ? length : location.z + radius;
		size_t h_max = location.y + radius > maxHeight ? maxHeight : location.y + radius;
		for (; i < i_max; i++) {
			for (; j < j_max; i++) {
				for (; h < h_max; h++) {
					if (glm::distance(glm::vec3(i, j, h), location) < radius) {
						voxelMap[i][h][j] = 0;
					}
				}
			}
		}
	}
<<<<<<< HEAD
	void Terrain::createCubeVector() {
=======
    */
	/*void Terrain::createCubeVector() {
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
		for (size_t i = 0; i < width; i++) {
			for (size_t j = 0; j < length; j++) {
				for (size_t h = 0; h < maxHeight; h++) {
					if (voxelMap[i][h][j]) {
						cubeTerrain.emplace_back(glm::vec3(i, h, j), 1);
					}
				}
			}
		}
<<<<<<< HEAD
	}
=======
	}*/
    /*
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
	Mesh Terrain::marchingCubesAlgorithm(glm::vec3 firstVoxel, glm::vec3 lastVoxel) { // implemented with the help of http://paulbourke.net/geometry/polygonise/ first link to a c++ code
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;
		for (size_t i = firstVoxel.x; i < lastVoxel.x; i++) {
			for (size_t j = firstVoxel.z; j < lastVoxel.z; j++) {
				for (size_t h = firstVoxel.y; h < lastVoxel.y; h++) {
					marchingCube(&vertices,i, h, j);
				}
			}
		}
		for (size_t y = 0; y < length - 1; y++) {//height replaced by length so the compiler does#nt give an error! NO IDEA WHAT THE RIGHT ORDAA IS!
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
		return Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	void Terrain::marchingCube(std::vector<Vertex>* vertices, int x, int y, int z) { // defined by the far bottom right voxelIndex ->
		
		// @Tim: Here's the original c++ code: http://paulbourke.net/geometry/polygonise/marchingsource.cpp		
		//				and here is also the link to the website itself: http://paulbourke.net/geometry/polygonise/ 
		//				there is a library for download but since you didn't want to use any more libraries here is my so far poor attempt of implementing dat shiiiit!
		//float scale = 1;
		std::vector<bool>cube(8);

		//front square starting up-left going counterclockwise
		cube[0] = !x || y == maxHeight || !z		? 0 : voxelMap[x - 1][y + 1][z - 1];
		cube[1] = !x || !z							? 0 : voxelMap[x - 1][y][z - 1];
		cube[2] = !x								? 0 : voxelMap[x - 1][y][z];
		cube[3] = !x || y == maxHeight				? 0 : voxelMap[x - 1][y + 1][z];
		//back square starting up-left going counterclockwise
		cube[4] = y==maxHeight || !z				? 0 : voxelMap[x][y + 1][z - 1];
		cube[5] = !z								? 0 : voxelMap[x][y][z - 1];
		cube[6] =									voxelMap[x][y][z];
		cube[7] = y == maxHeight					? 0 : voxelMap[x][y + 1][z];

		int edgeFlagIndex = 0;
		for (size_t i = 0; i < 8; i++) {
			if (cube[i]) {
				edgeFlagIndex |= 1 << i;
			}
		}
		int edgeFlag = marchingCubes_CubeEdgeFlags[edgeFlagIndex];
		if (edgeFlag == 0) {
			return;
		}
		//Find the point of intersection of the surface with each edge COPYPASTED
		//Then find the normal to the surface at those points			COPYPASTED
		std::vector<glm::vec3>edge(12);
		std::vector<glm::vec3>edgeNorm(12);
		for (size_t i = 0; i < 12; i++) {
			if (edgeFlag & (1 << i)) {

				//magic happens here
<<<<<<< HEAD
				/*ORIGINAL CODE*/
					/* fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], 
                                                     afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);

                        asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                        asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                        asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                        vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
					*/
				/*END ORIGINAL CODE*//*
=======
				
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
			}
		}
		//Draw the triangles that were found. THere can be up to five per cube COPYPASTED
		int vertex;
		for (size_t i = 0; i < 5; i++) {
			if (marchingCubes_TriangleConnectionTable[edgeFlagIndex][i * 3] < 0) {
				break;
			}
			for (int j = 0; i < 3; i++) {
				vertex = marchingCubes_TriangleConnectionTable[edgeFlagIndex][3 * i + j];
				//calculate normals
				//calculate edgevertecies
				//calculate indices
				//calculate positions
<<<<<<< HEAD
				/*ORIGINAL CODE*/
					/*glColor3f(sColor.fX, sColor.fY, sColor.fZ);
					glNormal3f(asEdgeNorm[iVertex].fX, asEdgeNorm[iVertex].fY, asEdgeNorm[iVertex].fZ);
					glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);*/
				/*END ORIGINAL CODE
			}
		}
		(*vertices).push_back(Vertex{ glm::vec3(1,2,3),glm::vec3(1,2,3) });//well, the values should be changed
	}*/
=======
				
			}
		}
		(*vertices).push_back(Vertex{ glm::vec3(1,2,3),glm::vec3(1,2,3) });//well, the values should be changed
	}
    */
>>>>>>> fbe4901e87831b5cb768918db32ece4af68841a0
}