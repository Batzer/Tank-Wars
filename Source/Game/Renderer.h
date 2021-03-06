#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <vector>
#include "VoxelTerrain.h"

namespace tankwars {
    class Camera;
    struct MeshInstance;
    class ParticleSystem;
    class Hud;
    class SkyBox;

    class Renderer {
    public:
        Renderer();
        Renderer(const Renderer&) = delete;
        ~Renderer();
        Renderer& operator=(const Renderer&) = delete;

        void render();
        void setBackBufferSize(GLsizei width, GLsizei height);
        void attachCamera(size_t viewportIndex, const Camera& camera);
        void attachHud(size_t viewportIndex, const Hud& hud);
        void setSplitScreenEnabled(bool enabled);

        // Scene managment
        // TODO: Refactor this into a seperate class
        void setAmbientColor(const glm::vec3& color);
        void setLight(const glm::vec3& color, const glm::vec3& direction);
        void addSceneObject(const MeshInstance& instance);
        void removeSceneObject(const MeshInstance& instance);
        void setTerrain(const VoxelTerrain* terrain);
        void setSkyBox(const SkyBox* skyBox);
        void addParticleSystem(const ParticleSystem& particleSystem);
        void removeParticleSystem(const ParticleSystem& particleSystem);

        static constexpr size_t ViewportTop = 0;
        static constexpr size_t ViewportBottom = 1;

    private:
        void renderScene(const Camera& camera, GLint viewportX, GLint viewportY,
                         GLsizei viewportWidth, GLsizei viewportHeight, bool clearBackBuffer);
        void renderHud(const Hud& hud);

        // Window info
        GLsizei backBufferWidth;
        GLsizei backBufferHeight;

        // Shaders
        GLuint toonLightingVS;
        GLuint toonLightingFS;
        GLuint toonLightingProgram;
        GLuint toonTerrainVS;
        GLuint toonTerrainFS;
        GLuint toonTerrainProgram;
        GLuint outlineVS;
        GLuint outlineFS;
        GLuint outlineProgram;
        GLuint genShadowMapVS;
        GLuint genShadowMapFS;
        GLuint genShadowMapProgram;
        GLuint depthQuadVS;
        GLuint depthQuadFS;
        GLuint depthQuadProgram;
        GLuint particleBillboardVS;
        GLuint particleBillboardFS;
        GLuint particleBillboardProgram;
        GLuint hudSpriteVS;
        GLuint hudSpriteFS;
        GLuint hudSpriteProgram;
        GLuint skyBoxVS;
        GLuint skyBoxFS;
        GLuint skyBoxProgram;

        // Uniform locations
        GLint toonLightingModelMatrixLocation;
        GLint toonLightingInvTrModelMatrixLocation;
        GLint toonLightingViewProjMatrixLocation;
        GLint toonLightingLightMatrixLocation;
        GLint toonLightingAmbientColorLocation;
        GLint toonLightingDirToLightLocation;
        GLint toonLightingEyePosLocation;
        GLint toonLightingLightColorLocation;
        GLint toonLightingMaterialDiffuseLocation;
        GLint toonLightingMaterialSpecularLocation;
        GLint toonLightingSpecularExponentLocation;
        GLint toonLightingShadowMapLocation;

        //GLint toonTerrainModelMatrixLocation;
        //GLint toonTerrainInvTrModelMatrixLocation;
        GLint toonTerrainViewProjMatrixLocation;
        GLint toonTerrainLightMatrixLocation;
        GLint toonTerrainAmbientColorLocation;
        GLint toonTerrainDirToLightLocation;
        GLint toonTerrainEyePosLocation;
        GLint toonTerrainLightColorLocation;
        GLint toonTerrainMaterialDiffuseLocation;
        GLint toonTerrainMaterialSpecularLocation;
        GLint toonTerrainSpecularExponentLocation;
        GLint toonTerrainColorMapTopLocation;
        GLint toonTerrainColorMapSideLocation;
        GLint toonTerrainColorMapBottomLocation;
        GLint toonTerrainShadowMapLocation;

        GLint outlineModelMatrixLocation;
        GLint outlineViewProjMatrixLocation;

        GLint genShadowMapModelMatrixLocation;
        GLint genShadowMapViewProjMatrixLocation;

        GLint particleBillboardCameraRightLocation;
        GLint particleBillboardCameraUpLocation;
        GLint particleBillboardViewProjMatLocation;

        GLint hudSpriteDimensionsLocation;
        GLint hudSpriteTexDimensionsLocation;
        GLint hudSpriteTransparencyLocation;

        GLint skyBoxViewProjMatLocation;

        // Shadow mapping
        static constexpr GLsizei ShadowMapSize = 4096;
        GLuint shadowMap;
        GLuint shadowMapFBO;

        // Scene managment
        // TODO: Refactor this into a seperate class
        glm::vec3 ambientColor = { 0.2f, 0.2f, 0.2f };
        glm::vec3 lightDirection = { 0, -1, -1 };
        glm::vec3 lightColor = { 1, 1, 1 };
        std::vector<const MeshInstance*> sceneObjects;
        std::vector<const ParticleSystem*> particleSystems;
        const VoxelTerrain* terrain = nullptr;
        GLuint terrainTextureTop;
        GLuint terrainTextureSide;
        GLuint terrainTextureBottom;
        bool isSplitScreenEnabled = false;
        const Camera* cameraTop = nullptr;
        const Camera* cameraBottom = nullptr;
        const Hud* hudTop = nullptr;
        const Hud* hudBottom = nullptr;
        const SkyBox* skyBox = nullptr;
    };
}