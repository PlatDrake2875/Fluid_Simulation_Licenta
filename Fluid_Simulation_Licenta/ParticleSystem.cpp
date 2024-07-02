#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ShaderManager* shaderManager) : shaderManager(shaderManager), particleGenerator(nullptr), particleRenderer(nullptr) {
    InitParticleGenerator();
    ParticleGenerator::ParticleSpawnData spawnData = particleGenerator->GetSpawnData();
    GPUSort* gpuSorter = new GPUSort();
    particleRenderer = new ParticleRenderer(particleGenerator->GetParticleCount(), shaderManager->GetShader(), shaderManager->GetComputeShader(), gpuSorter, spawnData);
}

ParticleSystem::~ParticleSystem() {
    delete particleGenerator;
    delete particleRenderer;
}

void ParticleSystem::InitParticleGenerator() {
    float screenWidth = 1024.0f;
    float screenHeight = 300.0f;

    particleGenerator = new ParticleGenerator(
        10000,
        glm::vec2(10.0f, 10.0f),
        glm::vec2(screenWidth / 2, screenHeight / 2),
        glm::vec2(screenWidth, screenHeight),
        0.2f
    );
}

void ParticleSystem::UpdateParticles() {
    if (Type == SimulationType::SLOW){
        //std::cout << "slow\n";
        particleRenderer->UpdateParticlesSlow();
    }
    else
    {
        particleRenderer->UpdateParticlesHash();
    }
}

void ParticleSystem::DrawParticles() {
    particleRenderer->DrawParticles();
}

ParticleRenderer* ParticleSystem::GetParticleRenderer() const {
    return particleRenderer;
}

void ParticleSystem::ApplyFunctionToParticles(std::function<void(std::vector<glm::vec2>&, std::vector<glm::vec2>&, float)> func, float deltaTime) {
    particleRenderer->get_apply_set(func, deltaTime);

}