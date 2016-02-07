#include "Hud.h"

#include <utility>

#include <glm/gtc/type_ptr.hpp>

#include "GLTools.h"

namespace {
    const glm::vec3 quadVertices[4] = {
        { 0.0f, 0.0f, 0 },
        { 1.0f, 0.0f, 0 },
        { 0.0f, 1.0f, 0 },
        { 1.0f, 1.0f, 0 }
    };
}

namespace tankwars {
    Hud::Hud() {
        glGenBuffers(1, &quadVbo);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glBufferData(GL_ARRAY_BUFFER, 48, quadVertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
        glBindVertexArray(0); // Unbind so no unwanted information is recorded
    }

    Hud::Hud(Hud&& other) {
        *this = std::move(other);
    }

    Hud::~Hud() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &quadVbo);
    }

    Hud& Hud::operator=(Hud&& other) {
        std::swap(vao, other.vao);
        std::swap(quadVbo, other.quadVbo);
        sprites = std::move(other.sprites);
        return *this;
    }

    void Hud::addSprite(const HudSprite& sprite, size_t layer) {
        if (sprites.size() <= layer) {
            sprites.resize(layer + 1, nullptr);
        }

        sprites[layer] = &sprite;
    }

    void Hud::removeSprite(const HudSprite& sprite) {
        for (size_t i = 0; i < sprites.size(); i++) {
            if (sprites[i] == &sprite) {
                sprites[i] = nullptr;
                break;
            }
        }
    }

    void Hud::render(GLint dimensionsLoc, GLint texDimensionsLoc, GLint transparencyLoc) const {
        for (auto sprite : sprites) {
            if (!sprite) {
                continue;
            }

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite->texture);
            glBindVertexArray(vao);
    
            glm::vec4 dimensions(sprite->position, sprite->size);
            glUniform4fv(dimensionsLoc, 1, glm::value_ptr(dimensions));

            glm::vec4 texDimensions(sprite->texOffset, sprite->texSize);
            glUniform4fv(texDimensionsLoc, 1, glm::value_ptr(texDimensions));

            glUniform1f(transparencyLoc, sprite->transparency);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
}