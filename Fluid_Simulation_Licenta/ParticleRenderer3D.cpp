#include "ParticleRenderer3D.h"

ParticleRenderer3D::ParticleRenderer3D(size_t particleCount, Shader* shader, ComputeShader* computeShader, GPUSort* gpuSorter, const ParticleGenerator3D::ParticleSpawnData3D& spawnData)
    : shader(shader), computeShader(computeShader), gpuSorter(gpuSorter) {
    particleBuffers = new ParticleBuffers3D(particleCount, computeShader);
    InitParticleData(particleCount, spawnData);
    InitRenderBuffers();
}

ParticleRenderer3D::~ParticleRenderer3D() {
    delete particleBuffers;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleRenderer3D::InitRenderBuffers() {
    useComputeShader();
    shader->use();
    // Generate and bind the Vertex Array Object
    glGenVertexArrays(1, &VAO);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - GenVertexArrays");

    glBindVertexArray(VAO);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - BindVertexArray");

    // Reallocate position buffer
    glGenBuffers(1, &positionVBO);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - GenPositionBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - BindPositionBuffer");

    if (!particleData.positions.empty()) {
        glBufferData(GL_ARRAY_BUFFER, particleData.positions.size() * sizeof(glm::vec3), particleData.positions.data(), GL_DYNAMIC_DRAW);
        CheckGLError("ParticleRenderer3D::InitRenderBuffers - PositionBufferData");
    }
    else {
        std::cerr << "Error: particleData.positions is empty during buffer allocation." << std::endl;
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - PositionVertexAttribPointer");

    glEnableVertexAttribArray(0);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - EnablePositionVertexAttribArray");

    // Reallocate velocity buffer
    glGenBuffers(1, &velocityVBO);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - GenVelocityBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, velocityVBO);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - BindVelocityBuffer");

    if (!particleData.velocities.empty()) {
        glBufferData(GL_ARRAY_BUFFER, particleData.velocities.size() * sizeof(glm::vec3), particleData.velocities.data(), GL_DYNAMIC_DRAW);
        CheckGLError("ParticleRenderer3D::InitRenderBuffers - VelocityBufferData");
    }
    else {
        std::cerr << "Error: particleData.velocities is empty during buffer allocation." << std::endl;
    }

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - VelocityVertexAttribPointer");

    glEnableVertexAttribArray(1);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - EnableVelocityVertexAttribArray");

    // Unbind buffers and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - UnbindBuffer");

    glBindVertexArray(0);
    CheckGLError("ParticleRenderer3D::InitRenderBuffers - UnbindVertexArray");
}

void ParticleRenderer3D::InitParticleData(size_t particleCount, const ParticleGenerator3D::ParticleSpawnData3D& spawnData) {
    particleData.positions = spawnData.positions;
    particleData.velocities = spawnData.velocities;
    particleData.predictedPositions = spawnData.positions;

    particleData.densities.resize(particleCount);
    particleData.spatialIndices.resize(particleCount);
    particleData.spatialOffsets.resize(particleCount);

    useComputeShader();
    particleBuffers->UpdateData(particleData.positions, particleData.velocities, particleData.predictedPositions, particleData.densities);
    //gpuSorter->SetBuffers(particleData.spatialIndices, particleData.spatialOffsets);

    std::cout << "Initializing particle data. Position count: " << particleData.positions.size() << std::endl;
}

void ParticleRenderer3D::ResizeBuffers() {
    useComputeShader();
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, particleData.positions.size() * sizeof(glm::vec3), particleData.positions.data(), GL_DYNAMIC_DRAW);
    CheckGLError("ParticleRenderer3D::ResizeBuffers - Resize positionVBO");

    glBindBuffer(GL_ARRAY_BUFFER, velocityVBO);
    glBufferData(GL_ARRAY_BUFFER, particleData.velocities.size() * sizeof(glm::vec3), particleData.velocities.data(), GL_DYNAMIC_DRAW);
    CheckGLError("ParticleRenderer3D::ResizeBuffers - Resize velocityVBO");

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer
}

void ParticleRenderer3D::UpdateParticlesSlow() {
    CheckGLError("ParticleRenderer3D::UpdateParticlesSlow - Before BindBuffers");

    useComputeShader();
    CheckGLError("ParticleRenderer3D::UpdateParticlesSlow - Use Compute Shader Program");

    GLuint particleCount = static_cast<GLuint>(particleData.positions.size());
    if (!validateParticleData(particleCount, NumThreads)) return;

    computeShader->DispatchComputeShader(particleCount, NumThreads);
    RetrieveAndDebugData();

    updateBuffer(positionVBO, particleData.positions, "positions");
    updateBuffer(velocityVBO, particleData.velocities, "velocities");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("ParticleRenderer3D::UpdateParticlesSlow - BindBuffer 0");
}

void ParticleRenderer3D::UpdateParticlesHash() {
    CheckGLError("ParticleRenderer3D::UpdateParticlesHash - Before BindBuffers");

    useComputeShader();
    CheckGLError("ParticleRenderer3D::UpdateParticlesHash - Use Compute Shader Program");

    GLuint particleCount = static_cast<GLuint>(particleData.positions.size());
    if (!validateParticleData(particleCount, NumThreads)) return;

    // First pass
    computeShader->setBool("passType", false);
    computeShader->DispatchComputeShader(particleCount, NumThreads);

    // Sort particles
    particleBuffers->RetrieveSpatialData(particleData.spatialIndices, particleData.spatialOffsets);
    useComputeShader();
    gpuSorter->UpdateBuffers(particleData.spatialIndices, particleData.spatialOffsets);
    gpuSorter->Sort();
    gpuSorter->RetrieveSpatialData(particleData.spatialIndices, particleData.spatialOffsets);
    useComputeShader();
    particleBuffers->UpdateSpatialData(particleData.spatialIndices, particleData.spatialOffsets);

    // Apply physics
    computeShader->setBool("passType", true);
    computeShader->DispatchComputeShader(particleCount, NumThreads);

    RetrieveAndDebugData();

    updateBuffer(positionVBO, particleData.positions, "positions");
    updateBuffer(velocityVBO, particleData.velocities, "velocities");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CheckGLError("ParticleRenderer3D::UpdateParticlesHash - BindBuffer 0");
}

void ParticleRenderer3D::useComputeShader() {
    computeShader->use();
    GLuint computeShaderID = computeShader->ID;
    glUseProgram(computeShaderID);

    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != computeShaderID) {
        std::cerr << "Compute shader program is not currently active." << std::endl;
    }
}

bool ParticleRenderer3D::validateParticleData(GLuint particleCount, GLuint numThreads) {
    if (particleCount == 0 || numThreads == 0) {
        std::cerr << "Invalid particle count or thread size." << std::endl;
        return false;
    }
    return true;
}

void ParticleRenderer3D::addParticles(const std::vector<glm::vec3>& newPositions) {
    size_t newParticleCount = particleData.positions.size() + newPositions.size();

    // Reserve space for new particles
    particleData.positions.reserve(newParticleCount);
    particleData.velocities.reserve(newParticleCount);
    particleData.predictedPositions.reserve(newParticleCount);
    particleData.densities.reserve(newParticleCount);
    particleData.spatialIndices.reserve(newParticleCount);
    particleData.spatialOffsets.reserve(newParticleCount);

    // Add new particles
    particleData.positions.insert(particleData.positions.end(), newPositions.begin(), newPositions.end());
    particleData.velocities.insert(particleData.velocities.end(), newPositions.size(), glm::vec3(10.0f, 10.0f, 10.0f));
    particleData.predictedPositions.insert(particleData.predictedPositions.end(), newPositions.begin(), newPositions.end());

    particleData.densities.insert(particleData.densities.end(), newPositions.size(), glm::vec2(0.0f, 0.0f));
    particleData.spatialIndices.insert(particleData.spatialIndices.end(), newPositions.size(), glm::uvec3(0));
    particleData.spatialOffsets.insert(particleData.spatialOffsets.end(), newPositions.size(), 0);

    // Reinitialize buffers with new size
    ResizeBuffers();

    particleBuffers->InitBuffers(newParticleCount);
    particleBuffers->UpdateAllBuffers(particleData);

    std::cout << "Added " << newPositions.size() << " particles. Total particles: " << particleData.positions.size() << std::endl;
}

void ParticleRenderer3D::updateBuffer(GLuint buffer, const std::vector<glm::vec3>& data, const std::string& bufferName) {
    useComputeShader();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    CheckGLError("ParticleRenderer3D::UpdateParticles - Bind " + bufferName + " Buffer");

    // Check the current size of the buffer
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    // Calculate the new data size
    GLsizeiptr dataSize = data.size() * sizeof(glm::vec3);

    // If the buffer size is different from the data size, reallocate the buffer
    if (bufferSize != dataSize) {
        glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_DYNAMIC_DRAW);
        CheckGLError("ParticleRenderer3D::UpdateParticles - " + bufferName + " BufferData");
    }

    if (!data.empty()) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data.data());
        CheckGLError("ParticleRenderer3D::UpdateParticles - " + bufferName + " BufferSubData");
    }
    else {
        std::cerr << "Error: particleData." << bufferName << " is empty during buffer update." << std::endl;
    }
}


void ParticleRenderer3D::RetrieveAndDebugData() {
    useComputeShader();
    particleBuffers->RetrieveData(particleData.positions, particleData.velocities, particleData.predictedPositions, particleData.densities);
    CheckGLError("ParticleRenderer3D::RetrieveAndDebugData - RetrieveData");

    //particleBuffers->DebugBufferData();
    CheckGLError("ParticleRenderer3D::RetrieveAndDebugData - DebugBufferData");

    std::vector<glm::uint> debugValues;
    //particleBuffers->RetrieveDebugData(debugValues); // Uncomment to debug FluidSimulator Data
    CheckGLError("ParticleRenderer3D::RetrieveAndDebugData - RetrieveDebugData");
    //DebugAdditionalBufferData(debugValues); // Uncomment to debug FluidSimulation.comp compute shader Data
}

void ParticleRenderer3D::DrawParticles(Camera* camera) {
    shader->use();
    CheckGLError("ParticleRenderer3D::DrawParticles - Use Shader");

    // Set the matrices from the camera
    shader->setMat4("projection", camera->getProjectionMatrix());
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("model", glm::mat4(1.0f)); // Assuming the model matrix is identity for the bounding box

    glBindVertexArray(VAO);
    CheckGLError("ParticleRenderer3D::DrawParticles - BindVertexArray");

    glPointSize(5.0f);
    CheckGLError("ParticleRenderer3D::DrawParticles - PointSize");

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particleData.positions.size()));
    CheckGLError("ParticleRenderer3D::DrawParticles - DrawArrays");

    glBindVertexArray(0);
    CheckGLError("ParticleRenderer3D::DrawParticles - BindVertexArray 0");
}

void ParticleRenderer3D::get_apply_set(std::function<void(std::vector<glm::vec3>&, std::vector<glm::vec3>&, float)> func, float deltaTime) {
    std::vector<glm::vec3>& positions = particleData.positions;
    std::vector<glm::vec3>& velocities = particleData.velocities;

    func(positions, velocities, deltaTime);

    updateBuffer(positionVBO, positions, "positions");
    updateBuffer(velocityVBO, velocities, "velocities");
}


ParticleData3D& ParticleRenderer3D::GetParticleData() {
    return particleData;
}

ParticleBuffers3D* ParticleRenderer3D::GetParticleBuffers() const {
    return particleBuffers;
}

float ParticleRenderer3D::GetMaxVelocity() const {
    float maxVelocity = 0.0f;
    for (const auto& velocity : particleData.velocities) {
        maxVelocity = std::max(maxVelocity, glm::length(velocity));
    }
    return maxVelocity;
}

float ParticleRenderer3D::GetMaxAcceleration(float deltaTime) const {
    float maxAcceleration = 0.0f;
    for (size_t i = 0; i < particleData.velocities.size(); ++i) {
        glm::vec3 acceleration = particleData.velocities[i] / deltaTime;
        maxAcceleration = std::max(maxAcceleration, glm::length(acceleration));
    }
    return maxAcceleration;
}

void ParticleRenderer3D::DebugParticleData() {
    std::cout << "Particle Data Debug Info:" << std::endl;
    for (size_t i = 0; i < particleData.positions.size(); ++i) {
        std::cout << "Particle " << i << ":" << std::endl;
        std::cout << "  Position: (" << particleData.positions[i].x << ", " << particleData.positions[i].y << ", " << particleData.positions[i].z << ")" << std::endl;
        std::cout << "  Velocity: (" << particleData.velocities[i].x << ", " << particleData.velocities[i].y << ", " << particleData.velocities[i].z << ")" << std::endl;
        std::cout << "  Predicted Position: (" << particleData.predictedPositions[i].x << ", " << particleData.predictedPositions[i].y << ", " << particleData.predictedPositions[i].z << ")" << std::endl;
    }
}

void ParticleRenderer3D::DebugAdditionalBufferData(const std::vector<glm::uint>& debugValues) {
    std::cout << "Additional Buffer Data Debug Info:" << std::endl;
    for (size_t i = 0; i < debugValues.size(); i += 8) {
        glm::uint threadId = debugValues[i];
        float posX, posY, posZ, velX, velY, velZ, predPosX, predPosY, predPosZ;
        memcpy(&posX, &debugValues[i + 1], sizeof(float));
        memcpy(&posY, &debugValues[i + 2], sizeof(float));
        memcpy(&posZ, &debugValues[i + 3], sizeof(float));
        memcpy(&velX, &debugValues[i + 4], sizeof(float));
        memcpy(&velY, &debugValues[i + 5], sizeof(float));
        memcpy(&velZ, &debugValues[i + 6], sizeof(float));
        memcpy(&predPosX, &debugValues[i + 7], sizeof(float));
        memcpy(&predPosY, &debugValues[i + 8], sizeof(float));
        memcpy(&predPosZ, &debugValues[i + 9], sizeof(float));
        glm::uint marker = debugValues[i + 10];

        std::cout << "Debug Values for Thread " << threadId << ":" << std::endl;
        std::cout << "  Position X: " << posX << std::endl;
        std::cout << "  Position Y: " << posY << std::endl;
        std::cout << "  Position Z: " << posZ << std::endl;
        std::cout << "  Velocity X: " << velX << std::endl;
        std::cout << "  Velocity Y: " << velY << std::endl;
        std::cout << "  Velocity Z: " << velZ << std::endl;
        std::cout << "  Predicted Position X: " << predPosX << std::endl;
        std::cout << "  Predicted Position Y: " << predPosY << std::endl;
        std::cout << "  Predicted Position Z: " << predPosZ << std::endl;
        std::cout << "  Debug Marker: " << marker << std::endl;

        if (marker == 1) {
            std::cout << "  Debug Stage: Initial" << std::endl;
        }
        else if (marker == 2) {
            std::cout << "  Debug Stage: After Prediction" << std::endl;
        }
        else if (marker == 3) {
            std::cout << "  Debug Stage: Final" << std::endl;
        }
    }
}

void ParticleRenderer3D::CheckGLError(const std::string& operation) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}
