#include "ParticleBuffers3D.h"

ParticleBuffers3D::ParticleBuffers3D(size_t particleCount, ComputeShader* computeShader)
    : particleCount(particleCount), computeShader(computeShader) {
    InitBuffers(particleCount);
}

ParticleBuffers3D::~ParticleBuffers3D() {
    glDeleteBuffers(1, &positionsBuffer);
    glDeleteBuffers(1, &predictedPositionsBuffer);
    glDeleteBuffers(1, &velocitiesBuffer);
    glDeleteBuffers(1, &densitiesBuffer);
    glDeleteBuffers(1, &spatialIndicesBuffer);
    glDeleteBuffers(1, &spatialOffsetsBuffer);
    glDeleteBuffers(1, &debugBuffer);
}

void ParticleBuffers3D::InitBuffers(size_t particleCount) {
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

    initBuffer(positionsBuffer, 0, particleCount * sizeof(glm::vec3), "positions");
    initBuffer(predictedPositionsBuffer, 1, particleCount * sizeof(glm::vec3), "predictedPositions");
    initBuffer(velocitiesBuffer, 2, particleCount * sizeof(glm::vec3), "velocities");
    initBuffer(densitiesBuffer, 3, particleCount * sizeof(glm::vec2), "densities");
    initBuffer(spatialIndicesBuffer, 4, particleCount * sizeof(glm::uvec3), "spatialIndices");
    initBuffer(spatialOffsetsBuffer, 5, particleCount * sizeof(glm::uint), "spatialOffsets");
    initBuffer(debugBuffer, 6, particleCount * 8 * sizeof(glm::uint), "debug");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    std::cout << "Buffers initialized successfully." << std::endl;
}

void ParticleBuffers3D::UpdateData(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& velocities, const std::vector<glm::vec3>& predictedPositions, const std::vector<glm::vec2>& densities) {
    auto updateBuffer = [&](GLuint buffer, const auto& data, const std::string& errorMsg) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(data[0]), data.data());
        CheckGLError(errorMsg);
        };

    updateBuffer(positionsBuffer, positions, "Update positionsBuffer");
    updateBuffer(predictedPositionsBuffer, predictedPositions, "Update predictedPositionsBuffer");
    updateBuffer(velocitiesBuffer, velocities, "Update velocitiesBuffer");
    updateBuffer(densitiesBuffer, densities, "Update densitiesBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ParticleBuffers3D::UpdateSpatialData(const std::vector<glm::uvec3>& spatialIndices, const std::vector<glm::uint>& spatialOffsets) {
    auto updateBuffer = [&](GLuint buffer, const auto& data, const std::string& errorMsg) {
        using ValueType = typename std::decay<decltype(data[0])>::type;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(ValueType), data.data());
        CheckGLError(errorMsg);
        };

    updateBuffer(spatialIndicesBuffer, spatialIndices, "Update spatialIndicesBuffer");
    updateBuffer(spatialOffsetsBuffer, spatialOffsets, "Update spatialOffsetsBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ParticleBuffers3D::UpdateAllBuffers(const ParticleData3D& particleData) {
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

void ParticleBuffers3D::RetrieveData(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& velocities, std::vector<glm::vec3>& predictedPositions, std::vector<glm::vec2>& densities) {
    positions.resize(particleCount);
    velocities.resize(particleCount);
    predictedPositions.resize(particleCount);
    densities.resize(particleCount);

    auto retrieveBufferData = [this](GLuint buffer, auto& data, const std::string& bufferName) {
        using ValueType = typename std::remove_reference<decltype(data)>::type::value_type;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        ValueType* ptr = (ValueType*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(ValueType), GL_MAP_READ_BIT);
        if (ptr) {
            std::copy(ptr, ptr + data.size(), data.begin());
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

void ParticleBuffers3D::RetrieveSpatialData(std::vector<glm::uvec3>& spatialIndices, std::vector<glm::uint>& spatialOffsets) {
    spatialIndices.resize(particleCount);
    spatialOffsets.resize(particleCount);

    auto retrieveBufferData = [this](GLuint buffer, auto& data, const std::string& bufferName) {
        using ValueType = typename std::remove_reference<decltype(data)>::type::value_type;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        ValueType* ptr = (ValueType*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(ValueType), GL_MAP_READ_BIT);
        if (ptr) {
            std::copy(ptr, ptr + data.size(), data.begin());
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
        else {
            std::cerr << "Failed to map " << bufferName << " buffer." << std::endl;
        }
        CheckGLError("Retrieve " + bufferName + "Buffer");
        };

    retrieveBufferData(spatialIndicesBuffer, spatialIndices, "spatialIndices");
    retrieveBufferData(spatialOffsetsBuffer, spatialOffsets, "spatialOffsets");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

GLuint ParticleBuffers3D::GetSpatialOffsetsBuffer() const {
    return spatialOffsetsBuffer;
}

GLuint ParticleBuffers3D::GetSpatialIndicesBuffer() const {
    return spatialIndicesBuffer;
}

void ParticleBuffers3D::DebugBufferData() {
    std::vector<glm::vec3> positions, velocities, predictedPositions;
    std::vector<glm::vec2> densities;
    RetrieveData(positions, velocities, predictedPositions, densities);

    std::cout << "Particle Buffers Debug Info:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        std::cout << "Particle " << i << ":" << std::endl;
        std::cout << "  Position: (" << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << ")" << std::endl;
        std::cout << "  Velocity: (" << velocities[i].x << ", " << velocities[i].y << ", " << velocities[i].z << ")" << std::endl;
        std::cout << "  Predicted Position: (" << predictedPositions[i].x << ", " << predictedPositions[i].y << ", " << predictedPositions[i].z << ")" << std::endl;
        std::cout << "  Densities: " << densities[i].x << " " << densities[i].y << std::endl;
    }
}

void ParticleBuffers3D::CheckGLError(const std::string& operation) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}

void ParticleBuffers3D::RetrieveDebugData(std::vector<glm::uint>& debugValues) {
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

void ParticleBuffers3D::useComputeShader() {
    computeShader->use();
    GLuint computeShaderID = computeShader->ID;
    glUseProgram(computeShaderID);

    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != computeShaderID) {
        std::cerr << "Compute shader program is not currently active." << std::endl;
    }
}
