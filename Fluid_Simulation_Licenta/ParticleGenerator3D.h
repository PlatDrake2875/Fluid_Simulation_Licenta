#ifndef PARTICLEGENERATOR3D_H
#define PARTICLEGENERATOR3D_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <iostream>

class ParticleGenerator3D {
public:
    struct ParticleSpawnData3D {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;

        ParticleSpawnData3D(int count) : positions(count), velocities(count) {}
    };

    ParticleGenerator3D(int particleCount, glm::vec3 initialVelocity, glm::vec3 spawnCentre, glm::vec3 spawnSize, float jitterStr);
    ~ParticleGenerator3D();

    int GetParticleCount() const;
    ParticleSpawnData3D GetSpawnData();

private:
    void InitBuffers();
    void CalculateGridDimensions(int& numParticlesPerAxis) const;
    glm::vec3 CalculateSpawnPosition(int x, int y, int z, int numParticlesPerAxis, std::mt19937& rng,
        std::uniform_real_distribution<float>& dist) const;
    void UpdateBufferData(const std::vector<glm::vec3>& positions);
    void DebugParticle(const glm::vec3& position, const glm::vec3& velocity) const;
    void CheckGLError(const std::string& operation) const;

    int particleCount;
    glm::vec3 initialVelocity;
    glm::vec3 spawnCentre;
    glm::vec3 spawnSize;
    float jitterStr;

    GLuint VAO, VBO;
};

#endif // PARTICLEGENERATOR3D_H
