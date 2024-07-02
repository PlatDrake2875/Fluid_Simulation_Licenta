#include "GPUSort.h"

GPUSort::GPUSort() : indexBuffer(0), offsetBuffer(0) {
    sortComputeShader = new ComputeShader("shaders/BitonicMergeSort.comp");
}

GPUSort::~GPUSort() {
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &offsetBuffer);
    delete sortComputeShader;
}

void GPUSort::SetBuffers(const std::vector<glm::uvec3>& spatialIndices, const std::vector<glm::uint>& spatialOffsets) {
    sortComputeShader->use();
    if (!indexBuffer) {
        glGenBuffers(1, &indexBuffer);
        CheckGLError("GPUSort::SetBuffers - Gen indexBuffer");
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexBuffer);
    CheckGLError("GPUSort::SetBuffers - Bind indexBuffer (before data)");
    glBufferData(GL_SHADER_STORAGE_BUFFER, spatialIndices.size() * sizeof(glm::uvec3), spatialIndices.data(), GL_DYNAMIC_DRAW);
    CheckGLError("GPUSort::SetBuffers - Data indexBuffer");
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, indexBuffer);
    CheckGLError("GPUSort::SetBuffers - BindBase indexBuffer");

    /*if (!offsetBuffer) {
        glGenBuffers(1, &offsetBuffer);
        CheckGLError("GPUSort::SetBuffers - Gen offsetBuffer");
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetBuffer);
    CheckGLError("GPUSort::SetBuffers - Bind offsetBuffer (before data)");
    glBufferData(GL_SHADER_STORAGE_BUFFER, spatialOffsets.size() * sizeof(glm::uint), spatialOffsets.data(), GL_DYNAMIC_DRAW);
    CheckGLError("GPUSort::SetBuffers - Data offsetBuffer");
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, offsetBuffer);
    CheckGLError("GPUSort::SetBuffers - BindBase offsetBuffer");*/
}

void GPUSort::UpdateBuffers(const std::vector<glm::uvec3>& spatialIndices, const std::vector<glm::uint>& spatialOffsets) {
    sortComputeShader->use();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexBuffer);
    CheckGLError("GPUSort::UpdateBuffers - Bind indexBuffer (before update)");
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, spatialIndices.size() * sizeof(glm::uvec3), spatialIndices.data());
    CheckGLError("GPUSort::UpdateBuffers - Update indexBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetBuffer);
    CheckGLError("GPUSort::UpdateBuffers - Bind offsetBuffer (before update)");
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, spatialOffsets.size() * sizeof(glm::uint), spatialOffsets.data());
    CheckGLError("GPUSort::UpdateBuffers - Update offsetBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUSort::RetrieveSpatialData(std::vector<glm::uvec3>& spatialIndices, std::vector<glm::uint>& spatialOffsets) {
    spatialIndices.resize(spatialIndices.size());
    spatialOffsets.resize(spatialOffsets.size());

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
        CheckGLError("Retrieve " + bufferName + " Buffer");
        };

    sortComputeShader->use();
    retrieveBufferData(indexBuffer, spatialIndices, "indexBuffer");
    retrieveBufferData(offsetBuffer, spatialOffsets, "offsetBuffer");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void GPUSort::Sort() {
    sortComputeShader->use();

    int numEntries = 10000;  // Replace with actual number of entries
    int numStages = static_cast<int>(std::log2(NextPowerOfTwo(numEntries)));
    sortComputeShader->setInt("numEntries", numEntries);

    for (int stageIndex = 0; stageIndex < numStages; ++stageIndex) {
        for (int stepIndex = 0; stepIndex < stageIndex + 1; ++stepIndex) {
            int groupWidth = 1 << (stageIndex - stepIndex);
            int groupHeight = 2 * groupWidth - 1;
            sortComputeShader->setInt("groupWidth", groupWidth);
            sortComputeShader->setInt("groupHeight", groupHeight);
            sortComputeShader->setInt("stepIndex", stepIndex);
            sortComputeShader->DispatchComputeShader((numEntries + 127) / 128, 128);
            CheckGLError("GPUSort::Sort - DispatchComputeShader");
        }
    }
}

void GPUSort::SortAndCalculateOffsets() {
    sortComputeShader->use();
    sortComputeShader->setInt("stepIndex", 0); 
    Sort();

    sortComputeShader->use();
    sortComputeShader->setInt("stepIndex", -1);  // -1 -> offset calculation kernel
    sortComputeShader->DispatchComputeShader((indexBuffer + 127) / 128, 128);
    CheckGLError("GPUSort::SortAndCalculateOffsets - DispatchComputeShader");
}

int GPUSort::NextPowerOfTwo(int value) {
    return static_cast<int>(std::pow(2, std::ceil(std::log2(value))));
}

void GPUSort::CheckGLError(const std::string& operation) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}
