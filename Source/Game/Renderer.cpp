#include "Renderer.h"
#include "GLTools.h"
#include "MeshInstance.h"
#include "Material.h"
#include "StaticMesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace gp {
    Renderer::Renderer() {
        // Create graphics resources
        _vertexShader = createShaderFromFile("Content/Shaders/Basic.vsh", GL_VERTEX_SHADER);
        _fragmentShader = createShaderFromFile("Content/Shaders/Basic.fsh", GL_FRAGMENT_SHADER);
        _shaderProgram = createAndLinkProgram(_vertexShader, _fragmentShader);

        // Query uniform locations from the shader programs
        _modelMatrixLocation = glGetUniformLocation(_shaderProgram, "ModelMatrix");
        _invTrModelMatrixLocation = glGetUniformLocation(_shaderProgram, "InvTrModelMatrix");
        _viewProjMatrixLocation = glGetUniformLocation(_shaderProgram, "ViewProjMatrix");
        _ambientColorLocation = glGetUniformLocation(_shaderProgram, "AmbientColor");
        _lightColorLocation = glGetUniformLocation(_shaderProgram, "LightColor");
        _dirToLightLocation = glGetUniformLocation(_shaderProgram, "DirToLight");
        _materialDiffuseLocation = glGetUniformLocation(_shaderProgram, "MaterialDiffuse");

        // Init some common GL states
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepthf(1.0f);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    Renderer::Renderer(Renderer&& other) {
        *this = std::move(other);
    }

    Renderer::~Renderer() {
        glDeleteShader(_fragmentShader);
        glDeleteShader(_vertexShader);
        glDeleteProgram(_shaderProgram);
    }

    Renderer& Renderer::operator=(Renderer&& other) {
        std::swap(_vertexShader, other._vertexShader);
        std::swap(_fragmentShader, other._fragmentShader);
        std::swap(_shaderProgram, other._shaderProgram);
        _modelMatrixLocation = other._modelMatrixLocation;
        _invTrModelMatrixLocation = other._invTrModelMatrixLocation;
        _viewProjMatrixLocation = other._viewProjMatrixLocation;
        _ambientColorLocation = other._ambientColorLocation;
        _dirToLightLocation = other._dirToLightLocation;
        _lightColorLocation = other._lightColorLocation;
        _materialDiffuseLocation = other._materialDiffuseLocation;
        _ambientColor = other._ambientColor;
        _lightDirection = other._lightDirection;
        _lightColor = other._lightColor;
        _sceneObjects = std::move(other._sceneObjects);
        return *this;
    }

    void Renderer::renderScene(const glm::mat4& viewProjMatrix) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(_shaderProgram);

        glUniform3fv(_ambientColorLocation, 1, glm::value_ptr(_ambientColor));
        glUniform3fv(_lightColorLocation, 1, glm::value_ptr(_lightColor));
        glUniform3fv(_dirToLightLocation, 1, glm::value_ptr(-glm::normalize(_lightDirection)));
        glUniformMatrix4fv(_viewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

        for (auto sceneObject : _sceneObjects) {
            glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneObject->getModelMatrix()));
            glUniformMatrix4fv(_invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneObject->getInvTrModelMatrix()));
            glUniform3fv(_materialDiffuseLocation, 1, glm::value_ptr(sceneObject->getMaterial()->diffuse));

            sceneObject->getMesh()->bindBuffers();
            sceneObject->getMesh()->render();
        }
    }

    void Renderer::setAmbientColor(const glm::vec3& color) {
        _ambientColor = color;
    }

    void Renderer::setLight(const glm::vec3& color, const glm::vec3& direction) {
        _lightColor = color;
        _lightDirection = direction;
    }

    void Renderer::addSceneObject(const MeshInstance& instance) {
        _sceneObjects.push_back(&instance);
    }

    void Renderer::removeSceneObject(const MeshInstance& instance) {
        auto end = _sceneObjects.end();
        _sceneObjects.erase(std::remove(_sceneObjects.begin(), end, &instance), end);
    }
}