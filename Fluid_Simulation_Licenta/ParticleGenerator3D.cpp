#include "ParticleGenerator3D.h"

ParticleGenerator3D::ParticleGenerator3D(int particleCount, glm::vec3 initialVelocity, glm::vec3 spawnCentre, glm::vec3 spawnSize, float jitterStr)
    : particleCount(particleCount), initialVelocity(initialVelocity), spawnCentre(spawnCentre), spawnSize(spawnSize), jitterStr(jitterStr) {
    InitBuffers();
}

ParticleGenerator3D::~ParticleGenerator3D() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleGenerator3D::InitBuffers() {
    glGenVertexArrays(1, &VAO);
    CheckGLError("InitBuffers::glGenVertexArrays");
    glGenBuffers(1, &VBO);
    CheckGLError("InitBuffers::glGenBuffers");

    glBindVertexArray(VAO);
    CheckGLError("InitBuffers::glBindVertexArray");
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    CheckGLError("InitBuffers::glBindBuffer");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    CheckGLError("InitBuffers::glVertexAttribPointer");
    glEnableVertexAttribArray(0);
    CheckGLError("InitBuffers::glEnableVertexAttribArray");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("InitBuffers::glBindBuffer unbind");
    glBindVertexArray(0);
    CheckGLError("InitBuffers::glBindVertexArray unbind");
}

int ParticleGenerator3D::GetParticleCount() const {
    return particleCount;
}

ParticleGenerator3D::ParticleSpawnData3D ParticleGenerator3D::GetSpawnData() {
    int numParticlesPerAxis = static_cast<int>(std::cbrt(particleCount));
    ParticleSpawnData3D data(particleCount);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int i = 0;
    for (int x = 0; x < numParticlesPerAxis; ++x) {
        for (int y = 0; y < numParticlesPerAxis; ++y) {
            for (int z = 0; z < numParticlesPerAxis; ++z) {
                glm::vec3 spawnPos = CalculateSpawnPosition(x, y, z, numParticlesPerAxis, rng, dist);
                data.positions[i] = spawnPos;
                data.velocities[i] = initialVelocity;
                ++i;
            }
        }
    }

    UpdateBufferData(data.positions);

    return data;
}

void ParticleGenerator3D::CalculateGridDimensions(int& numParticlesPerAxis) const {
    numParticlesPerAxis = static_cast<int>(std::cbrt(particleCount));
}

glm::vec3 ParticleGenerator3D::CalculateSpawnPosition(int x, int y, int z, int numParticlesPerAxis, std::mt19937& rng,
    std::uniform_real_distribution<float>& dist) const {

    float tx = numParticlesPerAxis <= 1 ? 0.5f : static_cast<float>(x) / (numParticlesPerAxis - 1);
    float ty = numParticlesPerAxis <= 1 ? 0.5f : static_cast<float>(y) / (numParticlesPerAxis - 1);
    float tz = numParticlesPerAxis <= 1 ? 0.5f : static_cast<float>(z) / (numParticlesPerAxis - 1);

    glm::vec3 jitter = glm::vec3(dist(rng) - 0.5f, dist(rng) - 0.5f, dist(rng) - 0.5f) * jitterStr;

    glm::vec3 spawnPos = glm::vec3(tx, ty, tz) * spawnSize + jitter;
    spawnPos = glm::clamp(spawnPos, glm::vec3(0.0f), spawnSize) + (spawnCentre - spawnSize / 2.0f);

    return spawnPos;
}

void ParticleGenerator3D::UpdateBufferData(const std::vector<glm::vec3>& positions) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("UpdateBufferData");
}

void ParticleGenerator3D::DebugParticle(const glm::vec3& position, const glm::vec3& velocity) const {
    float speed = glm::length(velocity);
    std::cout << "Particle Position: (" << position.x << ", " << position.y << ", " << position.z << "), Speed: " << speed << "\n";

    glm::vec3 minBound = spawnCentre - spawnSize / 2.0f;
    glm::vec3 maxBound = spawnCentre + spawnSize / 2.0f;

    if (position.x < minBound.x || position.x > maxBound.x || position.y < minBound.y || position.y > maxBound.y || position.z < minBound.z || position.z > maxBound.z) {
        std::cerr << "ParticleGenerator3D::DebugParticle Error: Particle out of bounds!\n";
    }
}

void ParticleGenerator3D::CheckGLError(const std::string& operation) const {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "ParticleGenerator3D::CheckGLError OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}
