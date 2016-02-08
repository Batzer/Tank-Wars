#include "ParticleSystem.h"

#include <cassert>
#include <algorithm>

#include "GLTools.h"

namespace {
    const glm::vec3 quadVertices[4] = {
        { -0.5f, -0.5f, 0 },
        {  0.5f, -0.5f, 0 },
        { -0.5f,  0.5f, 0 },
        {  0.5f,  0.5f, 0 }
    };
}

namespace tankwars {
    ParticleSystem::ParticleSystem(std::size_t maxParticles, GLuint texture, const ParticleSystemConfig& config)
        : particles(maxParticles)
        , maxParticles(maxParticles)
        , texture(texture)
        , minParticleVelocity(config.defaultMinVelocity)
        , maxParticleVelocity(config.defaultMaxVelocity)
        , minParticleAcceleration(config.defaultMinAcceleration)
        , maxParticleAcceleration(config.defaultMaxAcceleration)
        , minParticleColor(config.defaultMinColor)
        , maxParticleColor(config.defaultMaxColor)
        , minParticleSize(config.defaultMinSize)
        , maxParticleSize(config.defaultMaxSize)
        , minParticleLifeTime(config.defaultMinLifeTime)
        , maxParticleLifeTime(config.defaultMaxLifeTime)
        , positionDist(-emitterRadius, emitterRadius)
        , velocityXDist(minParticleVelocity.x, maxParticleVelocity.x)
        , velocityYDist(minParticleVelocity.y, maxParticleVelocity.y)
        , velocityZDist(minParticleVelocity.z, maxParticleVelocity.z)
        , accelerationXDist(minParticleAcceleration.x, maxParticleAcceleration.x)
        , accelerationYDist(minParticleAcceleration.y, maxParticleAcceleration.y)
        , accelerationZDist(minParticleAcceleration.z, maxParticleAcceleration.z)
        , colorRDist(minParticleColor.r, maxParticleColor.r)
        , colorGDist(minParticleColor.g, maxParticleColor.g)
        , colorBDist(minParticleColor.b, maxParticleColor.b)
        , colorADist(minParticleColor.a, maxParticleColor.a)
        , sizeDist(minParticleSize, maxParticleSize)
        , lifeTimeDist(minParticleLifeTime, maxParticleLifeTime)
        , mt(rd())
    {
        particleInstanceData.reserve(maxParticles);

        glGenBuffers(1, &quadVbo);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glBufferData(GL_ARRAY_BUFFER, 48, quadVertices, GL_STATIC_DRAW);

        glGenBuffers(1, &instanceVbo);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleInstanceData) * maxParticles, nullptr, GL_STREAM_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
        glVertexAttribDivisor(0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 32, 0);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 32, bufferOffset(16));
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glBindVertexArray(0); // Unbind so no unwanted information is recorded
    }

    ParticleSystem::~ParticleSystem() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &quadVbo);
        glDeleteBuffers(1, &instanceVbo);
    }

    void ParticleSystem::emit(std::size_t count) {
        std::size_t amount;
        if (nextFreeParticle + count <= maxParticles) {
            amount = count;
        }
        else {
            amount = maxParticles - nextFreeParticle;
        }

        for (std::size_t i = 0; i < amount; i++) {
            glm::vec3 position = emitterPosition;
            if (emitterType == EmitterType::Sphere) {
                position.x = positionDist(mt);
                position.y = positionDist(mt);
                position.z = positionDist(mt);
            }

            particles[nextFreeParticle].position = position;
            particles[nextFreeParticle].velocity = glm::vec3(velocityXDist(mt), velocityYDist(mt), velocityZDist(mt));
            particles[nextFreeParticle].acceleration = glm::vec3(accelerationXDist(mt), accelerationYDist(mt), accelerationZDist(mt));
            particles[nextFreeParticle].color = glm::vec4(colorRDist(mt), colorGDist(mt), colorBDist(mt), colorADist(mt));
            particles[nextFreeParticle].size = sizeDist(mt);
            particles[nextFreeParticle].lifeTime = lifeTimeDist(mt);
            particles[nextFreeParticle].isAlive = true;
            nextFreeParticle++;
        }
    }

    void ParticleSystem::update(float delta, const std::function<void(Particle&)>& customUpdate) {
        for (std::size_t i = 0; i < nextFreeParticle; ) {
            auto& particle = particles[i];
            particle.velocity += particle.acceleration * delta;
            particle.position += particle.velocity * delta;
            particle.lifeTime -= delta;
            particle.isAlive = (particle.lifeTime > 0.0f);

            if (customUpdate) {
                customUpdate(particle);
            }

            if (!particle.isAlive) {
                std::swap(particles[i], particles[nextFreeParticle - 1]);
                nextFreeParticle--;
            }
            else {
                i++;
            }
        }

        // Generate particle instances
        particleInstanceData.clear();
        for (std::size_t i = 0; i < nextFreeParticle; i++) {
            auto& particle = particles[i];
            particleInstanceData.push_back({ glm::vec4(particle.position, particle.size), particle.color });
        }

        // Orphan the buffer and fill in the particles
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleInstanceData) * maxParticles, nullptr, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, nextFreeParticle * sizeof(ParticleInstanceData), particleInstanceData.data());
    }

    void ParticleSystem::render() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(nextFreeParticle));
    }

    void ParticleSystem::setParticleTexture(GLuint texture) {
        this->texture = texture;
    }

    void ParticleSystem::setEmitterPosition(const glm::vec3& position) {
        emitterPosition = position;
    }

    void ParticleSystem::setEmitterType(EmitterType type) {
        emitterType = type;
    }

    void ParticleSystem::setEmitterRadius(float radius) {
        emitterRadius = radius;
        positionDist = std::uniform_real_distribution<float>(-emitterRadius, emitterRadius);
    }

    void ParticleSystem::setParticleVelocityRange(const glm::vec3& min, const glm::vec3& max) {
        minParticleVelocity = min;
        maxParticleVelocity = max;
        velocityXDist = std::uniform_real_distribution<float>(minParticleVelocity.x, maxParticleVelocity.x);
        velocityYDist = std::uniform_real_distribution<float>(minParticleVelocity.y, maxParticleVelocity.y);
        velocityZDist = std::uniform_real_distribution<float>(minParticleVelocity.z, maxParticleVelocity.z);
    }

    void ParticleSystem::setParticleAccelerationRange(const glm::vec3& min, const glm::vec3& max) {
        minParticleAcceleration = min;
        maxParticleAcceleration = max;
        accelerationXDist = std::uniform_real_distribution<float>(minParticleAcceleration.x, maxParticleAcceleration.x);
        accelerationYDist = std::uniform_real_distribution<float>(minParticleAcceleration.y, maxParticleAcceleration.y);
        accelerationZDist = std::uniform_real_distribution<float>(minParticleAcceleration.z, maxParticleAcceleration.z);
    }

    void ParticleSystem::setParticleColorRange(const glm::vec4& min, const glm::vec4& max) {
        minParticleColor = min;
        maxParticleColor = max;
        colorRDist = std::uniform_real_distribution<float>(minParticleColor.r, maxParticleColor.r);
        colorGDist = std::uniform_real_distribution<float>(minParticleColor.g, maxParticleColor.g);
        colorBDist = std::uniform_real_distribution<float>(minParticleColor.b, maxParticleColor.b);
        colorADist = std::uniform_real_distribution<float>(minParticleColor.a, maxParticleColor.a);
    }

    void ParticleSystem::setParticleSizeRange(float min, float max) {
        minParticleSize = min;
        maxParticleSize = max;
        sizeDist = std::uniform_real_distribution<float>(minParticleSize, maxParticleSize);
    }

    void ParticleSystem::setParticleLifeTimeRange(float min, float max) {
        minParticleLifeTime = min;
        maxParticleLifeTime = max;
        lifeTimeDist = std::uniform_real_distribution<float>(minParticleLifeTime, maxParticleLifeTime);
    }
}