#include "Renderer.h"

#include <algorithm>
#include <cassert>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLTools.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "ParticleSystem.h"
#include "Camera.h"
#include "Hud.h"

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

        toonTerrainVS = createShaderFromFile("Content/Shaders/ToonTerrain.vsh", GL_VERTEX_SHADER);
        toonTerrainFS = createShaderFromFile("Content/Shaders/ToonTerrain.fsh", GL_FRAGMENT_SHADER);
        toonTerrainProgram = createAndLinkProgram(toonTerrainVS, toonTerrainFS);

        outlineVS = createShaderFromFile("Content/Shaders/Outline.vsh", GL_VERTEX_SHADER);
        outlineFS = createShaderFromFile("Content/Shaders/Outline.fsh", GL_FRAGMENT_SHADER);
        outlineProgram = createAndLinkProgram(outlineVS, outlineFS);

        genShadowMapVS = createShaderFromFile("Content/Shaders/GenShadowMap.vsh", GL_VERTEX_SHADER);
        genShadowMapFS = createShaderFromFile("Content/Shaders/GenShadowMap.fsh", GL_FRAGMENT_SHADER);
        genShadowMapProgram = createAndLinkProgram(genShadowMapVS, genShadowMapFS);

        depthQuadVS = createShaderFromFile("Content/Shaders/DepthQuad.vsh", GL_VERTEX_SHADER);
        depthQuadFS = createShaderFromFile("Content/Shaders/DepthQuad.fsh", GL_FRAGMENT_SHADER);
        depthQuadProgram = createAndLinkProgram(depthQuadVS, depthQuadFS);

        particleBillboardVS = createShaderFromFile("Content/Shaders/ParticleBillboard.vsh", GL_VERTEX_SHADER);
        particleBillboardFS = createShaderFromFile("Content/Shaders/ParticleBillboard.fsh", GL_FRAGMENT_SHADER);
        particleBillboardProgram = createAndLinkProgram(particleBillboardVS, particleBillboardFS);

        hudSpriteVS = createShaderFromFile("Content/Shaders/HudSprite.vsh", GL_VERTEX_SHADER);
        hudSpriteFS = createShaderFromFile("Content/Shaders/HudSprite.fsh", GL_FRAGMENT_SHADER);
        hudSpriteProgram = createAndLinkProgram(hudSpriteVS, hudSpriteFS);

        // Query uniform locations from the shader programs
        toonLightingModelMatrixLocation = glGetUniformLocation(toonLightingProgram, "ModelMatrix");
        toonLightingInvTrModelMatrixLocation = glGetUniformLocation(toonLightingProgram, "InvTrModelMatrix");
        toonLightingViewProjMatrixLocation = glGetUniformLocation(toonLightingProgram, "ViewProjMatrix");
        toonLightingLightMatrixLocation = glGetUniformLocation(toonLightingProgram, "LightMatrix");
        toonLightingAmbientColorLocation = glGetUniformLocation(toonLightingProgram, "AmbientColor");
        toonLightingLightColorLocation = glGetUniformLocation(toonLightingProgram, "LightColor");
        toonLightingDirToLightLocation = glGetUniformLocation(toonLightingProgram, "DirToLight");
        toonLightingEyePosLocation = glGetUniformLocation(toonLightingProgram, "EyePos");
        toonLightingMaterialDiffuseLocation = glGetUniformLocation(toonLightingProgram, "MaterialDiffuse");
        toonLightingMaterialSpecularLocation = glGetUniformLocation(toonLightingProgram, "MaterialSpecular");
        toonLightingSpecularExponentLocation = glGetUniformLocation(toonLightingProgram, "SpecularExponent");
        toonLightingShadowMapLocation = glGetUniformLocation(toonLightingProgram, "ShadowMap");

        toonTerrainModelMatrixLocation = glGetUniformLocation(toonTerrainProgram, "ModelMatrix");
        toonTerrainInvTrModelMatrixLocation = glGetUniformLocation(toonTerrainProgram, "InvTrModelMatrix");
        toonTerrainViewProjMatrixLocation = glGetUniformLocation(toonTerrainProgram, "ViewProjMatrix");
        toonTerrainLightMatrixLocation = glGetUniformLocation(toonTerrainProgram, "LightMatrix");
        toonTerrainAmbientColorLocation = glGetUniformLocation(toonTerrainProgram, "AmbientColor");
        toonTerrainLightColorLocation = glGetUniformLocation(toonTerrainProgram, "LightColor");
        toonTerrainDirToLightLocation = glGetUniformLocation(toonTerrainProgram, "DirToLight");
        toonTerrainEyePosLocation = glGetUniformLocation(toonTerrainProgram, "EyePos");
        toonTerrainMaterialDiffuseLocation = glGetUniformLocation(toonTerrainProgram, "MaterialDiffuse");
        toonTerrainMaterialSpecularLocation = glGetUniformLocation(toonTerrainProgram, "MaterialSpecular");
        toonTerrainSpecularExponentLocation = glGetUniformLocation(toonTerrainProgram, "SpecularExponent");
        toonTerrainShadowMapLocation = glGetUniformLocation(toonTerrainProgram, "ShadowMap");
        toonTerrainColorMapTopLocation = glGetUniformLocation(toonTerrainProgram, "ColorMapTop");
        toonTerrainColorMapSideLocation = glGetUniformLocation(toonTerrainProgram, "ColorMapSide");
        toonTerrainColorMapBottomLocation = glGetUniformLocation(toonTerrainProgram, "ColorMapBottom");

        outlineModelMatrixLocation = glGetUniformLocation(outlineProgram, "ModelMatrix");
        outlineViewProjMatrixLocation = glGetUniformLocation(outlineProgram, "ViewProjMatrix");

        genShadowMapModelMatrixLocation = glGetUniformLocation(genShadowMapProgram, "ModelMatrix");
        genShadowMapViewProjMatrixLocation = glGetUniformLocation(genShadowMapProgram, "ViewProjMatrix");

        particleBillboardCameraRightLocation = glGetUniformLocation(particleBillboardProgram, "CameraRight");
        particleBillboardCameraUpLocation = glGetUniformLocation(particleBillboardProgram, "CameraUp");
        particleBillboardViewProjMatLocation = glGetUniformLocation(particleBillboardProgram, "ViewProjMat");

        hudSpriteDimensionsLocation = glGetUniformLocation(hudSpriteProgram, "Dimensions");
        hudSpriteTexDimensionsLocation = glGetUniformLocation(hudSpriteProgram, "TexDimensions");
        hudSpriteTransparencyLocation = glGetUniformLocation(hudSpriteProgram, "Transparency");

        // Create shadow map
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, ShadowMapSize, ShadowMapSize,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
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

        // Load textures
        terrainTextureTop = createTextureFromFile("Content/Textures/terrain_test.png", true);
        terrainTextureSide = createTextureFromFile("Content/Textures/terrain_test.png", true);
        terrainTextureBottom = createTextureFromFile("Content/Textures/terrain_test.png", true);

        float aniso = 0.0f;
        glBindTexture(GL_TEXTURE_2D, terrainTextureTop);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

        glBindTexture(GL_TEXTURE_2D, terrainTextureSide);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

        glBindTexture(GL_TEXTURE_2D, terrainTextureBottom);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
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
        /*modelMatrixLocation = other.modelMatrixLocation;
        invTrModelMatrixLocation = other.invTrModelMatrixLocation;
        viewProjMatrixLocation = other.viewProjMatrixLocation;
        ambientColorLocation = other.ambientColorLocation;
        dirToLightLocation = other.dirToLightLocation;
        eyePosLocation = other.eyePosLocation;
        lightColorLocation = other.lightColorLocation;
        materialDiffuseLocation = other.materialDiffuseLocation;
        materialSpecularLocation = other.materialSpecularLocation;
        specularExponentLocation = other.specularExponentLocation;*/
        ambientColor = other.ambientColor;
        lightDirection = other.lightDirection;
        lightColor = other.lightColor;
        sceneObjects = std::move(other.sceneObjects);
        return *this;
    }

    void Renderer::render() {
        assert(cameraTop != nullptr);

        if (isSplitScreenEnabled) {
            assert(cameraBottom != nullptr);

            renderScene(*cameraBottom, 0, 0, backBufferWidth, backBufferHeight / 2, true);
            renderHud(*hudBottom);
            renderScene(*cameraTop, 0, backBufferHeight / 2, backBufferWidth, backBufferHeight / 2, false);
            renderHud(*hudTop);
        }
        else {
            renderScene(*cameraTop, 0, 0, backBufferWidth, backBufferHeight, true);
            renderHud(*hudTop);
        }
    }

    void Renderer::setBackBufferSize(GLsizei width, GLsizei height) {
        backBufferWidth = width;
        backBufferHeight = height;
    }

    void Renderer::attachCamera(size_t viewportIndex, const Camera& camera) {
        assert(viewportIndex == 0 || viewportIndex == 1);

        if (viewportIndex == 0) {
            cameraTop = &camera;
        }
        else if (viewportIndex == 1) {
            cameraBottom = &camera;
        }
    }

    void Renderer::attachHud(size_t viewportIndex, const Hud& hud) {
        assert(viewportIndex == 0 || viewportIndex == 1);

        if (viewportIndex == 0) {
            hudTop = &hud;
        }
        else if (viewportIndex == 1) {
            hudBottom = &hud;
        }
    }

    void Renderer::setSplitScreenEnabled(bool enabled) {
        isSplitScreenEnabled = enabled;
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

    void Renderer::addParticleSystem(const ParticleSystem& particleSystem) {
        particleSystems.push_back(&particleSystem);
    }

    void Renderer::removeParticleSystem(const ParticleSystem& particleSystem) {
        auto end = particleSystems.end();
        particleSystems.erase(std::remove(particleSystems.begin(), end, &particleSystem), end);
    }

    void Renderer::renderScene(const Camera& camera, GLint viewportX, GLint viewportY,
                               GLsizei viewportWidth, GLsizei viewportHeight, bool clearBackBuffer) {
        const auto& cameraPos = camera.position;
        const auto& viewMatrix = camera.getViewMatrix();
        const auto& viewProjMatrix = camera.getViewProjMatrix();

        // Render scene to shadow map
        glViewport(0, 0, ShadowMapSize, ShadowMapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(genShadowMapProgram);

        auto lightViewMatrix = glm::lookAt(cameraPos - lightDirection * 50.0f, cameraPos, glm::vec3(0, 1, 0));
        auto lightProjMatrix = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 150.0f);
        auto lightMatrix = lightProjMatrix * lightViewMatrix;
        glUniformMatrix4fv(genShadowMapViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));

        for (auto sceneObject : sceneObjects) {
            glUniformMatrix4fv(genShadowMapModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(sceneObject->modelMatrix));
            sceneObject->mesh->render();
        }

        // Bind backbuffer and clear
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        if (clearBackBuffer) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

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
        //
        // Render terrain
        glCullFace(GL_BACK);
        glUseProgram(toonTerrainProgram);
        glUniform3fv(toonTerrainAmbientColorLocation, 1, glm::value_ptr(ambientColor));
        glUniform3fv(toonTerrainLightColorLocation, 1, glm::value_ptr(lightColor));
        glUniform3fv(toonTerrainDirToLightLocation, 1, glm::value_ptr(-glm::normalize(lightDirection)));
        glUniform3fv(toonTerrainEyePosLocation, 1, glm::value_ptr(cameraPos));
        glUniformMatrix4fv(toonTerrainViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
        glUniformMatrix4fv(toonTerrainLightMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(toonTerrainShadowMapLocation, 0);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(toonTerrainColorMapTopLocation, 1);
        glBindTexture(GL_TEXTURE_2D, terrainTextureTop);

        glActiveTexture(GL_TEXTURE2);
        glUniform1i(toonTerrainColorMapSideLocation, 2);
        glBindTexture(GL_TEXTURE_2D, terrainTextureSide);

        glActiveTexture(GL_TEXTURE3);
        glUniform1i(toonTerrainColorMapBottomLocation, 3);
        glBindTexture(GL_TEXTURE_2D, terrainTextureBottom);

        glUniformMatrix4fv(toonTerrainModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(toonTerrainInvTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniform3fv(toonTerrainMaterialDiffuseLocation, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
        glUniform3fv(toonTerrainMaterialSpecularLocation, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
        glUniform1f(toonTerrainSpecularExponentLocation, 1.0f);
        terrain->render();

        // Render the meshes
        glCullFace(GL_BACK);
        glUseProgram(toonLightingProgram);
        glUniform3fv(toonLightingAmbientColorLocation, 1, glm::value_ptr(ambientColor));
        glUniform3fv(toonLightingLightColorLocation, 1, glm::value_ptr(lightColor));
        glUniform3fv(toonLightingDirToLightLocation, 1, glm::value_ptr(-glm::normalize(lightDirection)));
        glUniform3fv(toonLightingEyePosLocation, 1, glm::value_ptr(cameraPos));
        glUniformMatrix4fv(toonLightingViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
        glUniformMatrix4fv(toonLightingLightMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(toonLightingShadowMapLocation, 0);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        for (auto sceneObject : sceneObjects) {
            assert(sceneObject->mesh);
            assert(sceneObject->material);

            const auto& modelMat = sceneObject->modelMatrix;
            auto invTrModelMat = glm::transpose(glm::inverse(modelMat));

            glUniformMatrix4fv(toonLightingModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
            glUniformMatrix4fv(toonLightingInvTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(invTrModelMat));
            glUniform3fv(toonLightingMaterialDiffuseLocation, 1, glm::value_ptr(sceneObject->material->diffuseColor));
            glUniform3fv(toonLightingMaterialSpecularLocation, 1, glm::value_ptr(sceneObject->material->specularColor));
            glUniform1f(toonLightingSpecularExponentLocation, sceneObject->material->specularExponent);

            sceneObject->mesh->render();
        }

        // Render particles
        glUseProgram(particleBillboardProgram);
        glUniformMatrix4fv(particleBillboardViewProjMatLocation, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

        glm::vec3 cameraRight(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        glm::vec3 cameraUp(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
        glUniform3fv(particleBillboardCameraRightLocation, 1, glm::value_ptr(cameraRight));
        glUniform3fv(particleBillboardCameraUpLocation, 1, glm::value_ptr(cameraUp));

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto particleSystem : particleSystems) {
            particleSystem->render();
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        /*
        glUseProgram(depthQuadProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        renderQuad();
        */
    }

    void Renderer::renderHud(const Hud& hud) {
        glUseProgram(hudSpriteProgram);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        hud.render(hudSpriteDimensionsLocation,
                   hudSpriteTexDimensionsLocation,
                   hudSpriteTransparencyLocation);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}