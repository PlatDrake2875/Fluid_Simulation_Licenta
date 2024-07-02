#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "ParticleGenerator.h"
#include "ParticleRenderer.h"
#include "ShaderManager.h"


enum class SimulationType {
    SLOW,
    HASH
};

class ShaderManager;

class ParticleSystem {
public:
    ParticleSystem(ShaderManager* shaderManager);
    ~ParticleSystem();

    void InitParticleGenerator();
    void UpdateParticles();
    void DrawParticles();

    ParticleRenderer* GetParticleRenderer() const;

    void ApplyFunctionToParticles(std::function<void(std::vector<glm::vec2>&, std::vector<glm::vec2>&, float)> func, float deltaTime);

    void setSimulationType(SimulationType value) { Type = value; }
    SimulationType getSimulationType() { return Type; }

private:
    ParticleGenerator* particleGenerator;
    ParticleRenderer* particleRenderer;
    ShaderManager* shaderManager;
    SimulationType Type = SimulationType::SLOW;
};

#endif // PARTICLESYSTEM_H
