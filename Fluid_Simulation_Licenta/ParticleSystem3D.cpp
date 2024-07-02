#include "ParticleSystem3D.h"

ParticleSystem3D::ParticleSystem3D(ShaderManager3D* shaderManager) : shaderManager(shaderManager), particleGenerator(nullptr), particleRenderer(nullptr) {
    InitParticleGenerator();
    ParticleGenerator3D::ParticleSpawnData3D spawnData = particleGenerator->GetSpawnData();
    GPUSort* gpuSorter = new GPUSort();
    particleRenderer = new ParticleRenderer3D(particleGenerator->GetParticleCount(), shaderManager->GetShader(), shaderManager->GetComputeShader(), gpuSorter, spawnData);
}

ParticleSystem3D::~ParticleSystem3D() {
    delete particleGenerator;
    delete particleRenderer;
}

void ParticleSystem3D::InitParticleGenerator() {
    float boxWidth = 1024.0f;
    float boxHeight = 300.0f;
    float boxDepth = 1024.0f;

    particleGenerator = new ParticleGenerator3D(
        10000,
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(boxWidth / 2, boxHeight / 2, boxDepth / 2),
        glm::vec3(boxWidth, boxHeight, boxDepth),
        0.1f
    );
}

void ParticleSystem3D::UpdateParticles() {
    if (Type == SimulationType3D::SLOW) {
        particleRenderer->UpdateParticlesSlow();
    }
    else {
        particleRenderer->UpdateParticlesHash();
    }
}

void ParticleSystem3D::DrawParticles(Camera* camera) {
    particleRenderer->DrawParticles(camera);
}

ParticleRenderer3D* ParticleSystem3D::GetParticleRenderer() const {
    return particleRenderer;
}

void ParticleSystem3D::ApplyFunctionToParticles(std::function<void(std::vector<glm::vec3>&, std::vector<glm::vec3>&, float)> func, float deltaTime) {
    particleRenderer->get_apply_set(func, deltaTime);
}

float ParticleSystem3D::GetMaxVelocity() const {
    // Assuming ParticleRenderer3D has a method to get the maximum velocity
    return particleRenderer->GetMaxVelocity();
}

float ParticleSystem3D::GetMaxAcceleration(float deltaTime) const {
    return particleRenderer->GetMaxAcceleration(deltaTime);
}
