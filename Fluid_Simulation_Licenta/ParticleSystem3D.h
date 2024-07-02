#ifndef PARTICLESYSTEM3D_H
#define PARTICLESYSTEM3D_H

#include "ParticleGenerator3D.h"
#include "ParticleRenderer3D.h"
#include "ShaderManager3D.h"
#include <functional>
#include <vector>
#include <glm/vec3.hpp>

enum class SimulationType3D {
    SLOW,
    HASH
};

class ShaderManager3D;

class ParticleSystem3D {
public:
    ParticleSystem3D(ShaderManager3D* shaderManager);
    ~ParticleSystem3D();

    void InitParticleGenerator();
    void UpdateParticles();
    void DrawParticles(Camera* camera);

    ParticleRenderer3D* GetParticleRenderer() const;

    void ApplyFunctionToParticles(std::function<void(std::vector<glm::vec3>&, std::vector<glm::vec3>&, float)> func, float deltaTime);

    float GetMaxVelocity() const;
    float GetMaxAcceleration(float deltaTime) const;

    void setSimulationType(SimulationType3D value) { Type = value; }
    SimulationType3D getSimulationType() { return Type; }

private:
    ParticleGenerator3D* particleGenerator;
    ParticleRenderer3D* particleRenderer;
    ShaderManager3D* shaderManager;
    SimulationType3D Type = SimulationType3D::SLOW;
};

#endif // PARTICLESYSTEM3D_H
