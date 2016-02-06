#pragma once

#include <vector>
#include <functional>
#include <random>
#include <memory>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace tankwars {
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        glm::vec4 color;
        float size = 0.0f;
        float lifeTime = 0.0f;
        bool isAlive = false;
    };

    struct ParticleSystemConfig {
        glm::vec3 defaultMinVelocity {-1, -1, -1};
        glm::vec3 defaultMaxVelocity {1, 1, 1};
        glm::vec3 defaultMinAcceleration {-1, -1, -1};
        glm::vec3 defaultMaxAcceleration {1, 1, 1};
        glm::vec4 defaultMinColor {0, 0, 0, 0};
        glm::vec4 defaultMaxColor {1, 1, 1, 1};
        float defaultMinSize = 0.1f;
        float defaultMaxSize = 0.2f;
        float defaultMinLifeTime = 0.1f;
        float defaultMaxLifeTime = 1.0f;
    };

    class ParticleSystem {
    public:
        ParticleSystem(size_t maxParticles, GLuint texture,
            const ParticleSystemConfig& config = ParticleSystemConfig());
        ~ParticleSystem();

        void emit(size_t count);
        void update(float delta, const std::function<void(Particle&)>& customUpdate = nullptr);
        void render() const;

        void setEmitterPosition(const glm::vec3& position);
        void setParticleTexture(GLuint texture);
        void setParticleVelocityRange(const glm::vec3& min, const glm::vec3& max);
        void setParticleAccelerationRange(const glm::vec3& min, const glm::vec3& max);
        void setParticleColorRange(const glm::vec4& min, const glm::vec4& max);
        void setParticleSizeRange(float min, float max);
        void setParticleLifeTimeRange(float min, float max);

    private:
        struct ParticleInstanceData {
            glm::vec4 pos; // w is size
            glm::vec4 color;
        };

        std::vector<Particle> particles;
        std::vector<ParticleInstanceData> particleInstanceData;
        size_t maxParticles;
        size_t nextFreeParticle = 0;
        glm::vec3 emitterPosition = glm::vec3(0, 0, 0);
        glm::vec3 minParticleVelocity;
        glm::vec3 maxParticleVelocity;
        glm::vec3 minParticleAcceleration;
        glm::vec3 maxParticleAcceleration;
        glm::vec4 minParticleColor;
        glm::vec4 maxParticleColor;
        float minParticleSize;
        float maxParticleSize;
        float minParticleLifeTime;
        float maxParticleLifeTime;

        std::uniform_real_distribution<float> velocityXDist;
        std::uniform_real_distribution<float> velocityYDist;
        std::uniform_real_distribution<float> velocityZDist;
        std::uniform_real_distribution<float> accelerationXDist;
        std::uniform_real_distribution<float> accelerationYDist;
        std::uniform_real_distribution<float> accelerationZDist;
        std::uniform_real_distribution<float> colorRDist;
        std::uniform_real_distribution<float> colorGDist;
        std::uniform_real_distribution<float> colorBDist;
        std::uniform_real_distribution<float> colorADist;
        std::uniform_real_distribution<float> sizeDist;
        std::uniform_real_distribution<float> lifeTimeDist;

        GLuint texture;
        GLuint quadVbo;
        GLuint instanceVbo;
        GLuint vao;

        std::random_device rd;
        std::mt19937 mt;
    };
}