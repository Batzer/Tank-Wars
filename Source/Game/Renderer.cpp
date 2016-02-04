#include "Renderer.h"

#include <algorithm>
#include <cassert>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLTools.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Terrain.h"

namespace {
    GLuint quadVAO = 0;
    GLuint quadVBO;
    void renderQuad()
    {
        if (quadVAO == 0)
        {
            GLfloat quadVertices[] = {
                // Positions        // Texture Coords
                0.25f,  1.0f, 0.0f,  0.0f, 1.0f,
                0.25f, 0.25f, 0.0f,  0.0f, 0.0f,
                1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
                1.0f, 0.25f, 0.0f,  1.0f, 0.0f,
            };
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}

namespace tankwars {
    Renderer::Renderer() {
        // Create shaders
        toonLightingVS = createShaderFromFile("Content/Shaders/ToonLighting.vsh", GL_VERTEX_SHADER);
        toonLightingFS = createShaderFromFile("Content/Shaders/ToonLighting.fsh", GL_FRAGMENT_SHADER);
        toonLightingProgram = createAndLinkProgram(toonLightingVS, toonLightingFS);

        outlineVS = createShaderFromFile("Content/Shaders/Outline.vsh", GL_VERTEX_SHADER);
        outlineFS = createShaderFromFile("Content/Shaders/Outline.fsh", GL_FRAGMENT_SHADER);
        outlineProgram = createAndLinkProgram(outlineVS, outlineFS);

        genShadowMapVS = createShaderFromFile("Content/Shaders/GenShadowMap.vsh", GL_VERTEX_SHADER);
        genShadowMapFS = createShaderFromFile("Content/Shaders/GenShadowMap.fsh", GL_FRAGMENT_SHADER);
        genShadowMapProgram = createAndLinkProgram(genShadowMapVS, genShadowMapFS);

        depthQuadVS = createShaderFromFile("Content/Shaders/DepthQuad.vsh", GL_VERTEX_SHADER);
        depthQuadFS = createShaderFromFile("Content/Shaders/DepthQuad.fsh", GL_FRAGMENT_SHADER);
        depthQuadProgram = createAndLinkProgram(depthQuadVS, depthQuadFS);

        // Query uniform locations from the shader programs
        modelMatrixLocation = glGetUniformLocation(toonLightingProgram, "ModelMatrix");
        invTrModelMatrixLocation = glGetUniformLocation(toonLightingProgram, "InvTrModelMatrix");
        viewProjMatrixLocation = glGetUniformLocation(toonLightingProgram, "ViewProjMatrix");
        lightMatrixLocation = glGetUniformLocation(toonLightingProgram, "LightMatrix");
        ambientColorLocation = glGetUniformLocation(toonLightingProgram, "AmbientColor");
        lightColorLocation = glGetUniformLocation(toonLightingProgram, "LightColor");
        dirToLightLocation = glGetUniformLocation(toonLightingProgram, "DirToLight");
        eyePosLocation = glGetUniformLocation(toonLightingProgram, "EyePos");
        materialDiffuseLocation = glGetUniformLocation(toonLightingProgram, "MaterialDiffuse");
        materialSpecularLocation = glGetUniformLocation(toonLightingProgram, "MaterialSpecular");
        specularExponentLocation = glGetUniformLocation(toonLightingProgram, "SpecularExponent");
        shadowMapLocation = glGetUniformLocation(toonLightingProgram, "ShadowMap");

        outlineModelMatrixLocation = glGetUniformLocation(outlineProgram, "ModelMatrix");
        outlineViewProjMatrixLocation = glGetUniformLocation(outlineProgram, "ViewProjMatrix");

        genShadowMapModelMatrixLocation = glGetUniformLocation(genShadowMapProgram, "ModelMatrix");
        genShadowMapViewProjMatrixLocation = glGetUniformLocation(genShadowMapProgram, "ViewProjMatrix");

        // Create shadow map
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowMapSize, ShadowMapSize,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

        glGenFramebuffers(1, &shadowMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapFBO, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
        glDeleteFramebuffers(1, &shadowMapFBO);
        glDeleteTextures(1, &shadowMap);
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

    void Renderer::renderScene(const glm::mat4& projMatrix, const glm::mat4& viewMatrix, const glm::vec3& cameraPos) {
        auto viewProjMatrix = projMatrix * viewMatrix;

        // Render scene to shadow map
        glViewport(0, 0, ShadowMapSize, ShadowMapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(genShadowMapProgram);
        
        auto lightViewMatrix = glm::lookAt(cameraPos - lightDirection * 50.0f, cameraPos, glm::vec3(0, 1, 0));
        auto lightProjMatrix = glm::ortho(-75.0f, 75.0f, -75.0f, 75.0f, -25.0f, 150.0f);
        auto lightMatrix = lightProjMatrix * lightViewMatrix;
        glUniformMatrix4fv(genShadowMapViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));

        for (auto sceneObject : sceneObjects) {
            glUniformMatrix4fv(genShadowMapModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneObject->modelMatrix));
            sceneObject->mesh->render();
        }

        // Bind backbuffer and clear
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, backBufferWidth, backBufferHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render outline
        glCullFace(GL_FRONT);
        glUseProgram(outlineProgram);
        glUniformMatrix4fv(outlineViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

        glUniformMatrix4fv(outlineModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        terrain->render();

        for (auto sceneObject : sceneObjects) {
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
        glUniformMatrix4fv(lightMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(shadowMapLocation, 0);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        
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

        /*
        glUseProgram(depthQuadProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        renderQuad();
        */
    }

    void Renderer::setBackBufferSize(GLsizei width, GLsizei height) {
        backBufferWidth = width;
        backBufferHeight = height;
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