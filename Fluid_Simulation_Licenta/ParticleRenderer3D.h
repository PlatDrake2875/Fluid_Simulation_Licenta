#ifndef PARTICLE_RENDERER3D_H
#define PARTICLE_RENDERER3D_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <functional>

#include "Shader.h"
#include "ParticleBuffers3D.h"
#include "ParticleGenerator3D.h"
#include "ComputeShader.h"
#include "ParticleData.h"
#include "GPUSort.h"
#include "Camera.h"

class ParticleRenderer3D {
public:
    ParticleRenderer3D(size_t particleCount, Shader* shader, ComputeShader* computeShader, GPUSort* gpuSorter, const ParticleGenerator3D::ParticleSpawnData3D& spawnData);
    ~ParticleRenderer3D();

    void UpdateParticlesSlow();
    void UpdateParticlesHash();
    void useComputeShader();
    bool validateParticleData(GLuint particleCount, GLuint numThreads);
    void addParticles(const std::vector<glm::vec3>& newPositions);
    void updateBuffer(GLuint buffer, const std::vector<glm::vec3>& data, const std::string& bufferName);
    void RetrieveAndDebugData();
    void DrawParticles(Camera* camera);

    void get_apply_set(std::function<void(std::vector<glm::vec3>&, std::vector<glm::vec3>&, float)> func, float deltaTime);

    ParticleData3D& GetParticleData();
    ParticleBuffers3D* GetParticleBuffers() const;
    float GetMaxVelocity() const;
    float GetMaxAcceleration(float deltaTime) const;
    void DebugParticleData();
    void DebugAdditionalBufferData(const std::vector<glm::uint>& debugValues);

private:
    Shader* shader;
    ComputeShader* computeShader;
    GPUSort* gpuSorter;
    ParticleBuffers3D* particleBuffers;
    ParticleData3D particleData;
    GLuint VAO, VBO;
    GLuint positionVBO, velocityVBO;

    static const int NumThreads = 64;

    void InitRenderBuffers();
    void InitParticleData(size_t particleCount, const ParticleGenerator3D::ParticleSpawnData3D& spawnData);
    void ResizeBuffers();
    void CheckGLError(const std::string& operation);
};

#endif // PARTICLE_RENDERER3D_H
