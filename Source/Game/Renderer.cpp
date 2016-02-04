#include "Renderer.h"

#include <algorithm>
#include <cassert>

#include <glm/gtc/type_ptr.hpp>

#include "GLTools.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Terrain.h"

namespace tankwars {
    Renderer::Renderer() {
        // Create graphics resources
        toonLightingVS = createShaderFromFile("Content/Shaders/ToonLighting.vsh", GL_VERTEX_SHADER);
        toonLightingFS = createShaderFromFile("Content/Shaders/ToonLighting.fsh", GL_FRAGMENT_SHADER);
        toonLightingProgram = createAndLinkProgram(toonLightingVS, toonLightingFS);

        outlineVS = createShaderFromFile("Content/Shaders/Outline.vsh", GL_VERTEX_SHADER);
        outlineFS = createShaderFromFile("Content/Shaders/Outline.fsh", GL_FRAGMENT_SHADER);
        outlineProgram = createAndLinkProgram(outlineVS, outlineFS);

        // Query uniform locations from the shader programs
        modelMatrixLocation = glGetUniformLocation(toonLightingProgram, "ModelMatrix");
        invTrModelMatrixLocation = glGetUniformLocation(toonLightingProgram, "InvTrModelMatrix");
        viewProjMatrixLocation = glGetUniformLocation(toonLightingProgram, "ViewProjMatrix");
        ambientColorLocation = glGetUniformLocation(toonLightingProgram, "AmbientColor");
        lightColorLocation = glGetUniformLocation(toonLightingProgram, "LightColor");
        dirToLightLocation = glGetUniformLocation(toonLightingProgram, "DirToLight");
        eyePosLocation = glGetUniformLocation(toonLightingProgram, "EyePos");
        materialDiffuseLocation = glGetUniformLocation(toonLightingProgram, "MaterialDiffuse");
        materialSpecularLocation = glGetUniformLocation(toonLightingProgram, "MaterialSpecular");
        specularExponentLocation = glGetUniformLocation(toonLightingProgram, "SpecularExponent");

        outlineModelMatrixLocation = glGetUniformLocation(outlineProgram, "ModelMatrix");
        outlineViewProjMatrixLocation = glGetUniformLocation(outlineProgram, "ViewProjMatrix");

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
        glDeleteShader(toonLightingFS);
        glDeleteShader(toonLightingVS);
        glDeleteShader(outlineFS);
        glDeleteShader(outlineVS);
        glDeleteProgram(toonLightingProgram);
        glDeleteProgram(outlineProgram);
    }

    Renderer& Renderer::operator=(Renderer&& other) {
        std::swap(toonLightingVS, other.toonLightingVS);
        std::swap(toonLightingFS, other.toonLightingFS);
        std::swap(toonLightingProgram, other.toonLightingProgram);
        modelMatrixLocation = other.modelMatrixLocation;
        invTrModelMatrixLocation = other.invTrModelMatrixLocation;
        viewProjMatrixLocation = other.viewProjMatrixLocation;
        ambientColorLocation = other.ambientColorLocation;
        dirToLightLocation = other.dirToLightLocation;
        eyePosLocation = other.eyePosLocation;
        lightColorLocation = other.lightColorLocation;
        materialDiffuseLocation = other.materialDiffuseLocation;
        materialSpecularLocation = other.materialSpecularLocation;
        specularExponentLocation = other.specularExponentLocation;
        ambientColor = other.ambientColor;
        lightDirection = other.lightDirection;
        lightColor = other.lightColor;
        sceneObjects = std::move(other.sceneObjects);
        return *this;
    }

    void Renderer::renderScene(const glm::mat4& viewProjMatrix, const glm::vec3& cameraPos) {
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render outline
        glCullFace(GL_FRONT);
        glUseProgram(outlineProgram);
        glUniformMatrix4fv(outlineViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

        glUniformMatrix4fv(outlineModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        terrain->render();

        for (auto sceneObject : sceneObjects) {
            assert(sceneObject->mesh);
            assert(sceneObject->material);

            glUniformMatrix4fv(outlineModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneObject->modelMatrix));
            sceneObject->mesh->render();
        }

        // Render colors
        glCullFace(GL_BACK);
        glUseProgram(toonLightingProgram);
        glUniform3fv(ambientColorLocation, 1, glm::value_ptr(ambientColor));
        glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
        glUniform3fv(dirToLightLocation, 1, glm::value_ptr(-glm::normalize(lightDirection)));
        glUniform3fv(eyePosLocation, 1, glm::value_ptr(cameraPos));
        glUniformMatrix4fv(viewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
        
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(glm::vec3(0, 0.6f, 0)));
        glUniform3fv(materialSpecularLocation, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
        glUniform1f(specularExponentLocation, 1.0f);
        terrain->render();

        for (auto sceneObject : sceneObjects) {
            assert(sceneObject->mesh);
            assert(sceneObject->material);

            const auto& modelMat = sceneObject->modelMatrix;
            auto invTrModelMat = glm::transpose(glm::inverse(modelMat));

            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glUniformMatrix4fv(invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(invTrModelMat));
            glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(sceneObject->material->diffuseColor));
            glUniform3fv(materialSpecularLocation, 1, glm::value_ptr(sceneObject->material->specularColor));
            glUniform1f(specularExponentLocation, sceneObject->material->specularExponent);

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

    void Renderer::setTerrain(const VoxelTerrain* terrain) {
        this->terrain = terrain;
    }
}