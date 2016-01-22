#include "Renderer.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "GLTools.h"
#include "Mesh.h"
#include "Terrain.h"

namespace tankwars {
    Renderer::Renderer() {
        // Create graphics resources
        vertexShader = createShaderFromFile("Content/Shaders/Basic.vsh", GL_VERTEX_SHADER);
        fragmentShader = createShaderFromFile("Content/Shaders/Basic.fsh", GL_FRAGMENT_SHADER);
        shaderProgram = createAndLinkProgram(vertexShader, fragmentShader);

        // Query uniform locations from the shader programs
        modelMatrixLocation = glGetUniformLocation(shaderProgram, "ModelMatrix");
        invTrModelMatrixLocation = glGetUniformLocation(shaderProgram, "InvTrModelMatrix");
        viewProjMatrixLocation = glGetUniformLocation(shaderProgram, "ViewProjMatrix");
        ambientColorLocation = glGetUniformLocation(shaderProgram, "AmbientColor");
        lightColorLocation = glGetUniformLocation(shaderProgram, "LightColor");
        dirToLightLocation = glGetUniformLocation(shaderProgram, "DirToLight");
        materialDiffuseLocation = glGetUniformLocation(shaderProgram, "MaterialDiffuse");

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
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteProgram(shaderProgram);
    }

    Renderer& Renderer::operator=(Renderer&& other) {
        std::swap(vertexShader, other.vertexShader);
        std::swap(fragmentShader, other.fragmentShader);
        std::swap(shaderProgram, other.shaderProgram);
        modelMatrixLocation = other.modelMatrixLocation;
        invTrModelMatrixLocation = other.invTrModelMatrixLocation;
        viewProjMatrixLocation = other.viewProjMatrixLocation;
        ambientColorLocation = other.ambientColorLocation;
        dirToLightLocation = other.dirToLightLocation;
        lightColorLocation = other.lightColorLocation;
        materialDiffuseLocation = other.materialDiffuseLocation;
        ambientColor = other.ambientColor;
        lightDirection = other.lightDirection;
        lightColor = other.lightColor;
        sceneObjects = std::move(other.sceneObjects);
        return *this;
    }

    void Renderer::renderScene(const glm::mat4& viewProjMatrix) {
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniform3fv(ambientColorLocation, 1, glm::value_ptr(ambientColor));
        glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
        glUniform3fv(dirToLightLocation, 1, glm::value_ptr(-glm::normalize(lightDirection)));
        glUniformMatrix4fv(viewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(glm::vec3(0, 1, 0)));
        terrain->render();

        for (auto sceneObject : sceneObjects) {
            auto modelMat = buildMatrixFromTransform(sceneObject->transform);
            auto invTrModelMat = glm::transpose(glm::inverse(modelMat));
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glUniformMatrix4fv(invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(invTrModelMat));
            glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(sceneObject->material->diffuseColor));

            sceneObject->mesh->render();
        }
    }

    void Renderer::setAmbientColor(const glm::vec3& color) {
        ambientColor = color;
    }

    void Renderer::setLight(const glm::vec3& color, const glm::vec3& direction) {
        lightColor = color;
        lightDirection = direction;
    }

    void Renderer::addSceneObject(const MeshInstance& instance) {
        sceneObjects.push_back(&instance);
    }

    void Renderer::removeSceneObject(const MeshInstance& instance) {
        auto end = sceneObjects.end();
        sceneObjects.erase(std::remove(sceneObjects.begin(), end, &instance), end);
    }

    void Renderer::setTerrain(const VoxelChunk* terrain) {
        this->terrain = terrain;
    }
}