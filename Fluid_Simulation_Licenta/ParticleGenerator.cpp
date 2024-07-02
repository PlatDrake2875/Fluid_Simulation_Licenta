#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(int particleCount, glm::vec2 initialVelocity, glm::vec2 spawnCentre, glm::vec2 spawnSize, float jitterStr)
    : particleCount(particleCount), initialVelocity(initialVelocity), spawnCentre(spawnCentre), spawnSize(spawnSize), jitterStr(jitterStr) {
    InitBuffers();
}

void ParticleGenerator::InitBuffers() {
    glGenVertexArrays(1, &VAO);
    CheckGLError("InitBuffers::glGenVertexArrays");
    glGenBuffers(1, &VBO);
    CheckGLError("InitBuffers::glGenBuffers");

    glBindVertexArray(VAO);
    CheckGLError("InitBuffers::glBindVertexArray");
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    CheckGLError("InitBuffers::glBindBuffer");
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    CheckGLError("InitBuffers::glVertexAttribPointer");
    glEnableVertexAttribArray(0);
    CheckGLError("InitBuffers::glEnableVertexAttribArray");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("InitBuffers::glBindBuffer unbind");
    glBindVertexArray(0);
    CheckGLError("InitBuffers::glBindVertexArray unbind");
}

int ParticleGenerator::GetParticleCount() const {
    return particleCount;
}

ParticleGenerator::ParticleSpawnData ParticleGenerator::GetSpawnData() {
    ParticleSpawnData data(particleCount);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.14f);

    int numX, numY;
    CalculateGridDimensions(numX, numY);

    for (int y = 0, i = 0; y < numY && i < particleCount; ++y) {
        for (int x = 0; x < numX && i < particleCount; ++x) {
            glm::vec2 spawnPos = CalculateSpawnPosition(x, y, numX, numY, rng, dist, angleDist);
            data.positions[i] = spawnPos;
            data.velocities[i] = initialVelocity;
            //DebugParticle(spawnPos, initialVelocity);  // Uncomment to debug particle position and speed
            ++i;
        }
    }

    //UpdateBufferData(data.positions);

    return data;
}

void ParticleGenerator::CalculateGridDimensions(int& numX, int& numY) const {
    float sx = spawnSize.x;
    float sy = spawnSize.y;

    float aspectRatio = sx / sy;
    float area = aspectRatio * static_cast<float>(particleCount);
    float offsetX = (sx - sy) / (2 * sy);
    float sqrtTerm = static_cast<float>(std::sqrt(area + std::pow(offsetX, 2)));

    numX = static_cast<int>(std::ceil(sqrtTerm - offsetX));
    numY = static_cast<int>(std::ceil(static_cast<float>(particleCount) / numX));
}

glm::vec2 ParticleGenerator::CalculateSpawnPosition(int x, int y, int numX, int numY, std::mt19937& rng,
    std::uniform_real_distribution<float>& dist, std::uniform_real_distribution<float>& angleDist) const {

    // Calculate normalized position (tx, ty) in the grid based on the index (x, y) and (numX, numY)
    float tx = numX <= 1 ? 0.5f : static_cast<float>(x) / (numX - 1);
    float ty = numY <= 1 ? 0.5f : static_cast<float>(y) / (numY - 1);

    // Generate a random angle for jitter direction
    float angle = angleDist(rng);
    glm::vec2 dir(std::cos(angle), std::sin(angle));

    // Calculate jitter offset based on the random direction and magnitude
    glm::vec2 jitter = dir * jitterStr * (dist(rng) - 0.5f);

    // Calculate the initial spawn position in the grid and apply jitter
    glm::vec2 spawnPos = glm::vec2(tx * spawnSize.x, ty * spawnSize.y) + jitter;

    // Clamp the spawn position to be within the spawn size bounds and center it around spawnCentre
    spawnPos = glm::clamp(spawnPos, glm::vec2(0.0f), spawnSize) + (spawnCentre - spawnSize / 2.0f);

    return spawnPos;
}

void ParticleGenerator::UpdateBufferData(const std::vector<glm::vec2>& positions) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("UpdateBufferData");
}

void ParticleGenerator::DebugParticle(const glm::vec2& position, const glm::vec2& velocity) const {
    float speed = glm::length(velocity);
    std::cout << "Particle Position: (" << position.x << ", " << position.y << "), Speed: " << speed << "\n";

    float minX = spawnCentre.x - spawnSize.x / 2.0f;
    float maxX = spawnCentre.x + spawnSize.x / 2.0f;
    float minY = spawnCentre.y - spawnSize.y / 2.0f;
    float maxY = spawnCentre.y + spawnSize.y / 2.0f;

    if (position.x < minX || position.x > maxX || position.y < minY || position.y > maxY) {
        std::cerr << "ParticleGenerator::DebugParticle Error: Particle out of bounds!\n";
    }
}

void ParticleGenerator::CheckGLError(const std::string& operation) const {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "ParticleGenerator::CheckGLError OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}
