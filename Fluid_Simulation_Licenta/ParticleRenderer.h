#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <functional>
#include <glm/vec2.hpp>

#include "Shader.h"
#include "ParticleBuffers.h"
#include "ParticleGenerator.h"
#include "ComputeShader.h"
#include "ParticleData.h"


class ParticleRenderer {
public:
    ParticleRenderer(size_t particleCount, Shader* shader, ComputeShader* computeShader, const ParticleGenerator::ParticleSpawnData& spawnData);
    ~ParticleRenderer();

    void UpdateParticles();
    void useComputeShader();
    bool validateParticleData(GLuint particleCount, GLuint numThreads);
    void addParticles(const std::vector<glm::vec2>& newPositions);
    void updateBuffer(GLuint buffer, const std::vector<glm::vec2>& data, const std::string& bufferName);
    void RetrieveAndDebugData();
    void DrawParticles();

    void get_apply_set(std::function<void(std::vector<glm::vec2>&, std::vector<glm::vec2>&, float)> func, float deltaTime);

    ParticleData& GetParticleData();
    ParticleBuffers* GetParticleBuffers() const;
    void DebugParticleData();
    void DebugAdditionalBufferData(const std::vector<glm::uint>& debugValues);

private:
    Shader* shader;
    ComputeShader* computeShader;
    ParticleBuffers* particleBuffers;
    ParticleData particleData;
    GLuint VAO, VBO;
    GLuint positionVBO, velocityVBO;

    static const int NumThreads = 64;

    void InitRenderBuffers();
    void InitParticleData(size_t particleCount, const ParticleGenerator::ParticleSpawnData& spawnData);
    void ResizeBuffers();
    void CheckGLError(const std::string& operation);
};

#endif // PARTICLE_RENDERER_H
