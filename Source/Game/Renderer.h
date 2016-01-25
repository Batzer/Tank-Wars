#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <vector>
#include "VoxelTerrain.h"

namespace tankwars {
    struct MeshInstance;

    class Renderer {
    public:
        Renderer();
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&& other);
        ~Renderer();

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&& other);

        void renderScene(const glm::mat4& viewProjMatrix, const glm::vec3& cameraPos);

        // Scene managment
        // TODO: Refactor this into a seperate class
        void setAmbientColor(const glm::vec3& color);
        void setLight(const glm::vec3& color, const glm::vec3& direction);
        void addSceneObject(const MeshInstance& instance);
        void removeSceneObject(const MeshInstance& instance);
        void setTerrain(const VoxelTerrain* terrain);

    private:
        // Shaders
        GLuint vertexShader;
        GLuint fragmentShader;
        GLuint shaderProgram;

        // Uniform locations
        GLint modelMatrixLocation;
        GLint invTrModelMatrixLocation;
        GLint viewProjMatrixLocation;
        GLint ambientColorLocation;
        GLint dirToLightLocation;
        GLint eyePosLocation;
        GLint lightColorLocation;
        GLint materialDiffuseLocation;
        GLint materialSpecularLocation;
        GLint specularExponentLocation;

        // Scene managment
        // TODO: Refactor this into a seperate class
        glm::vec3 ambientColor = { 0.2f, 0.2f, 0.2f };
        glm::vec3 lightDirection = { 0, -1, -1 };
        glm::vec3 lightColor = { 1, 1, 1 };
        std::vector<const MeshInstance*> sceneObjects;
        const VoxelTerrain* terrain = nullptr;
    };
}