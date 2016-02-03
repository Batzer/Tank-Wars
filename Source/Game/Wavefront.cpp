#include "Wavefront.h"

#include <fstream>
#include <algorithm>

namespace {
    struct FaceVertex {
        uint32_t positionIndex;
        uint32_t texCoordIndex;
        uint32_t normalIndex;
    };

    std::vector<std::string> splitString(const std::string& str, char delim) {
        std::vector<std::string> tokens;
        std::string::size_type curr = 0;
        std::string::size_type last = 0;

        while ((curr = str.find(delim, last)) != std::string::npos) {
            auto token = str.substr(last, curr - last);
            tokens.push_back(std::move(token));
            last = curr + 1;
        }

        tokens.push_back(str.substr(last));
        return tokens;
    }
}

namespace tankwars {
    WavefrontModel readWavefrontFromFile(const std::string& path, bool revertWindings) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Attempt to open non-existent wavefront file <" + path + ">");
        }

        WavefrontModel model;
        std::vector<glm::vec3> tempPositions;
        std::vector<glm::vec2> tempTexCoords;
        std::vector<glm::vec3> tempNormals;
        std::vector<FaceVertex> faceVertices;
        std::vector<uint32_t> triangleCache;

        for (std::string line; std::getline(file, line); ) {
            // Ignore empty, comment, smoothing group, object group and polygon group lines
            if (line.empty() || line[0] == '#' || line[0] == 's' || line[0] == 'g' || line[0] == 'o') {
                continue;
            }

            auto tokens = splitString(line, ' ');
            
            if (tokens[0] == "v") {
                auto x = std::stof(tokens[1]);
                auto y = std::stof(tokens[2]);
                auto z = std::stof(tokens[3]);
                tempPositions.emplace_back(x, y, z);
            }
            else if (tokens[0] == "vt") {
                auto u = std::stof(tokens[1]);
                auto v = std::stof(tokens[2]);
                tempTexCoords.emplace_back(u, v);
            }
            else if (tokens[0] == "vn") {
                auto x = std::stof(tokens[1]);
                auto y = std::stof(tokens[2]);
                auto z = std::stof(tokens[3]);
                tempNormals.emplace_back(x, y, z);
            }
            else if (tokens[0] == "f") {
                triangleCache.clear();

                for (size_t i = 1; i < tokens.size(); i++) {
                    auto vertexIndices = splitString(tokens[i], '/');
                    auto hasTexCoord = (vertexIndices.size() >= 2 && !vertexIndices[1].empty());
                    auto hasNormal = (vertexIndices.size() >= 3 && !vertexIndices[2].empty());

                    auto posIndex = static_cast<uint32_t>(std::stoi(vertexIndices[0]));
                    uint32_t texCoordIndex = hasTexCoord ? static_cast<uint32_t>(std::stoi(vertexIndices[1])) : 0;
                    uint32_t normalIndex = hasNormal ? static_cast<uint32_t>(std::stoi(vertexIndices[2])) : 0;

                    bool needNewVertex = true;
                    uint32_t vertexIndex = 0;

                    for (size_t index = 0; index < faceVertices.size(); index++) {
                        const auto& faceVertex = faceVertices[index];

                        if (faceVertex.positionIndex == posIndex) {
                            auto texCoordMatch = (texCoordIndex == faceVertex.texCoordIndex);
                            auto normalMatch = (normalIndex == faceVertex.normalIndex);

                            if (hasTexCoord && hasNormal) {
                                needNewVertex = !(texCoordMatch && normalMatch);
                            }
                            else if (hasTexCoord && !hasNormal) {
                                needNewVertex = !texCoordMatch;
                            }
                            else if (!hasTexCoord && hasNormal) {
                                needNewVertex = !normalMatch;
                            }
                            else {
                                needNewVertex = false;
                            }   
                        }

                        if (!needNewVertex) {
                            vertexIndex = static_cast<uint16_t>(index);
                            break;
                        }
                    }

                    if (needNewVertex) {
                        faceVertices.push_back({ posIndex, normalIndex, texCoordIndex });
                        model.positions.push_back(tempPositions[posIndex - 1]);

                        if (hasTexCoord) {
                            model.texCoords.push_back(tempTexCoords[texCoordIndex - 1]);
                        }

                        if (hasNormal) {
                            model.normals.push_back(tempNormals[normalIndex - 1]);
                        }

                        triangleCache.push_back(static_cast<uint16_t>(faceVertices.size() - 1));
                    }
                    else {
                        triangleCache.push_back(vertexIndex);
                    }
                }

                // TODO: Check if the triangles are CC or CCW
                for (size_t i = 1; i < triangleCache.size() - 1; i++) {
                    model.triangles.push_back(triangleCache[0]);
                    model.triangles.push_back(triangleCache[i]);
                    model.triangles.push_back(triangleCache[i + 1]);
                }
            }
        }

        if (revertWindings) {
            for (size_t i = 0; i < model.triangles.size(); i += 3) {
                std::swap(model.triangles[i + 1], model.triangles[i + 2]);
            }
        }

        // TODO: Generate normals if none were defined in the file

        return model;
    }

    Mesh createMeshFromWavefront(const WavefrontModel& model) {
        std::vector<Vertex> vertices;
        for (size_t i = 0; i < model.positions.size(); i++) {
            vertices.push_back({ model.positions[i], model.normals[i] });
        }

        return Mesh(vertices.data(), vertices.size(), model.triangles.data(), model.triangles.size());
    }
}