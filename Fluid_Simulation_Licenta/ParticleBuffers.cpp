#include "ParticleBuffers.h"

ParticleBuffers::ParticleBuffers(size_t particleCount)
    : particleCount(particleCount) {
    InitBuffers(particleCount);
}

ParticleBuffers::~ParticleBuffers() {
    glDeleteBuffers(1, &positionsBuffer);
    glDeleteBuffers(1, &predictedPositionsBuffer);
    glDeleteBuffers(1, &velocitiesBuffer);
    glDeleteBuffers(1, &densitiesBuffer);
    glDeleteBuffers(1, &spatialIndicesBuffer);
    glDeleteBuffers(1, &spatialOffsetsBuffer);
    glDeleteBuffers(1, &debugBuffer);
}

void ParticleBuffers::InitBuffers(size_t particleCount) {
    this->particleCount = particleCount;
    std::cout << "Initializing buffers..." << std::endl;

    if (!glewIsSupported("GL_VERSION_4_3")) {
        std::cerr << "OpenGL 4.3 is not supported. Cannot use SSBOs." << std::endl;
        return;
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error before buffer initialization: " << std::hex << err << std::dec << std::endl;
        return;
    }

    auto initBuffer = [this](GLuint& buffer, GLuint index, GLsizeiptr size, const std::string& bufferName) {
        glGenBuffers(1, &buffer);
        CheckGLError("Gen " + bufferName + "Buffer");
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        CheckGLError("Bind " + bufferName + "Buffer (before data)");
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        CheckGLError("Data " + bufferName + "Buffer");
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        CheckGLError("BindBase " + bufferName + "Buffer");
        };

    initBuffer(positionsBuffer, 0, particleCount * sizeof(glm::vec2), "positions");
    initBuffer(predictedPositionsBuffer, 1, particleCount * sizeof(glm::vec2), "predictedPositions");
    initBuffer(velocitiesBuffer, 2, particleCount * sizeof(glm::vec2), "velocities");
    initBuffer(densitiesBuffer, 3, particleCount * sizeof(glm::vec2), "densities");
    initBuffer(spatialIndicesBuffer, 4, particleCount * sizeof(glm::uvec3), "spatialIndices");
    initBuffer(spatialOffsetsBuffer, 5, particleCount * sizeof(glm::uint), "spatialOffsets");
    initBuffer(debugBuffer, 6, particleCount * 8 * sizeof(glm::uint), "debug");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    std::cout << "Buffers initialized successfully." << std::endl;
}



void ParticleBuffers::UpdateData(const std::vector<glm::vec2>& positions, const std::vector<glm::vec2>& velocities, const std::vector<glm::vec2>& predictedPositions, const std::vector<glm::vec2>& densities) {
    auto updateBuffer = [&](GLuint buffer, const std::vector<glm::vec2>& data, const std::string& errorMsg) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(glm::vec2), data.data());
        CheckGLError(errorMsg);
        };

    updateBuffer(positionsBuffer, positions, "Update positionsBuffer");
    updateBuffer(predictedPositionsBuffer, predictedPositions, "Update predictedPositionsBuffer");
    updateBuffer(velocitiesBuffer, velocities, "Update velocitiesBuffer");
    updateBuffer(densitiesBuffer, densities, "Update densitiesBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ParticleBuffers::UpdateAllBuffers(const ParticleData& particleData) {
    auto updateBuffer = [&](GLuint buffer, const auto& data, const std::string& errorMsg) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_DYNAMIC_DRAW);
        CheckGLError(errorMsg);
        };

    updateBuffer(positionsBuffer, particleData.positions, "Update positionsBuffer");
    updateBuffer(predictedPositionsBuffer, particleData.predictedPositions, "Update predictedPositionsBuffer");
    updateBuffer(velocitiesBuffer, particleData.velocities, "Update velocitiesBuffer");
    updateBuffer(densitiesBuffer, particleData.densities, "Update densitiesBuffer");
    updateBuffer(spatialIndicesBuffer, particleData.spatialIndices, "Update spatialIndicesBuffer");
    updateBuffer(spatialOffsetsBuffer, particleData.spatialOffsets, "Update spatialOffsetsBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void ParticleBuffers::RetrieveData(std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities, std::vector<glm::vec2>& predictedPositions, std::vector<glm::vec2>& densities) {
    positions.resize(particleCount);
    velocities.resize(particleCount);
    predictedPositions.resize(particleCount);
    densities.resize(particleCount);

    auto retrieveBufferData = [this](GLuint buffer, std::vector<glm::vec2>& data, const std::string& bufferName) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glm::vec2* ptr = (glm::vec2*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, particleCount * sizeof(glm::vec2), GL_MAP_READ_BIT);
        if (ptr) {
            std::copy(ptr, ptr + particleCount, data.begin());
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
        else {
            std::cerr << "Failed to map " << bufferName << " buffer." << std::endl;
        }
        CheckGLError("Retrieve " + bufferName + "Buffer");
        };

    retrieveBufferData(positionsBuffer, positions, "positions");
    retrieveBufferData(velocitiesBuffer, velocities, "velocities");
    retrieveBufferData(predictedPositionsBuffer, predictedPositions, "predictedPositions");
    retrieveBufferData(densitiesBuffer, densities, "densities");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}



GLuint ParticleBuffers::GetSpatialOffsetsBuffer() const {
    return spatialOffsetsBuffer;
}

GLuint ParticleBuffers::GetSpatialIndicesBuffer() const {
    return spatialIndicesBuffer;

}
void ParticleBuffers::DebugBufferData() {
    std::vector<glm::vec2> positions, velocities, predictedPositions, densities;
    RetrieveData(positions, velocities, predictedPositions, densities);

    std::cout << "Particle Buffers Debug Info:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        std::cout << "Particle " << i << ":" << std::endl;
        std::cout << "  Position: (" << positions[i].x << ", " << positions[i].y << ")" << std::endl;
        std::cout << "  Velocity: (" << velocities[i].x << ", " << velocities[i].y << ")" << std::endl;
        std::cout << "  Predicted Position: (" << predictedPositions[i].x << ", " << predictedPositions[i].y << ")" << std::endl;
        std::cout << "  Densities: (" << densities[i].x << ", " << densities[i].y << ")" << std::endl;
    }
}

void ParticleBuffers::CheckGLError(const std::string& operation) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}

void ParticleBuffers::RetrieveDebugData(std::vector<glm::uint>& debugValues) {
    debugValues.resize(particleCount * 8);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugBuffer);
    glm::uint* debugValuesPtr = (glm::uint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, debugValues.size() * sizeof(glm::uint), GL_MAP_READ_BIT);
    if (debugValuesPtr) {
        std::copy(debugValuesPtr, debugValuesPtr + debugValues.size(), debugValues.begin());
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    else {
        std::cerr << "Failed to map debug buffer." << std::endl;
    }
    CheckGLError("RetrieveDebugData");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
