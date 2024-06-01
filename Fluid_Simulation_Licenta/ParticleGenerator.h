#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <random>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Shader.h"

class ParticleGenerator {
public:
    struct ParticleSpawnData {
        std::vector<glm::vec2> positions;
        std::vector<glm::vec2> velocities;

        ParticleSpawnData(int num) {
            positions.resize(num);
            velocities.resize(num);
        }
    };

    ParticleGenerator(int particleCount, glm::vec2 initialVelocity, glm::vec2 spawnCentre, glm::vec2 spawnSize, float jitterStr);

    ParticleSpawnData GetSpawnData();
    void DrawParticles(Shader* shader);
    int GetParticleCount() const;

private:
    int particleCount;
    glm::vec2 initialVelocity;
    glm::vec2 spawnCentre;
    glm::vec2 spawnSize;
    float jitterStr;

    GLuint VAO, VBO;

    void InitBuffers();
    void CalculateGridDimensions(int& numX, int& numY) const;
    glm::vec2 CalculateSpawnPosition(int x, int y, int numX, int numY, std::mt19937& rng,
        std::uniform_real_distribution<float>& dist, std::uniform_real_distribution<float>& angleDist) const;
    void UpdateBufferData(const std::vector<glm::vec2>& positions);
    void DebugParticle(const glm::vec2& position, const glm::vec2& velocity) const;
    void CheckGLError(const std::string& operation) const;
};

#endif // PARTICLE_GENERATOR_H