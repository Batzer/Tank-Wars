#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <vector>

namespace gp {
    class MeshInstance;

    class Renderer {
    public:
        Renderer();
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&& other);
        ~Renderer();

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&& other);

        void renderScene(const glm::mat4& viewProjMatrix);

        // Scene managment
        // TODO: Refactor this into a seperate class
        void setAmbientColor(const glm::vec3& color);
        void setLight(const glm::vec3& color, const glm::vec3& direction);
        void addSceneObject(const MeshInstance& instance);
        void removeSceneObject(const MeshInstance& instance);

    private:
        // Shaders
        GLuint _vertexShader;
        GLuint _fragmentShader;
        GLuint _shaderProgram;

        // Uniform locations
        GLint _modelMatrixLocation;
        GLint _invTrModelMatrixLocation;
        GLint _viewProjMatrixLocation;
        GLint _ambientColorLocation;
        GLint _dirToLightLocation;
        GLint _lightColorLocation;
        GLint _materialDiffuseLocation;

        // Scene managment
        // TODO: Refactor this into a seperate class
        glm::vec3 _ambientColor = { 0.2f, 0.2f, 0.2f };
        glm::vec3 _lightDirection = { 1, -1, -1 };
        glm::vec3 _lightColor = { 1, 1, 1 };
        std::vector<const MeshInstance*> _sceneObjects;
    };
}