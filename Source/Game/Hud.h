#pragma once

#include <cstddef>
#include <vector>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace tankwars {
    struct HudSprite {
        GLuint texture;
        glm::vec2 position{ 0.0f, 0.0f };
        glm::vec2 size{ 0.1f, 0.1f };
        glm::vec2 texOffset{ 0.0f, 0.0f };
        glm::vec2 texSize{ 1.0f, 1.0f };
        float transparency = 1.0f;
    };

    class Hud {
    public:
        Hud();
        Hud(const Hud&) = delete;
        Hud(Hud&& other);
        ~Hud();

        Hud& operator=(const Hud&) = delete;
        Hud& operator=(Hud&& other);

        // Layer 0 is rendered first, so layer 1 will cover layer 0. And so on.
        void addSprite(const HudSprite& sprite, size_t layer);
        void removeSprite(const HudSprite& sprite);
        
        // This will be called by the renderer
        void render(GLint dimensionsLoc, GLint texDimensionsLoc, GLint transparencyLoc) const;

    private:
        GLuint quadVbo;
        GLuint vao;
        std::vector<const HudSprite*> sprites;
    };
}