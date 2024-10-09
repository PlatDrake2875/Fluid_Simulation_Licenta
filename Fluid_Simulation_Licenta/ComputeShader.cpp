#include "ComputeShader.h"
#include <iostream>
#include <fstream>
#include <sstream>

ComputeShader::ComputeShader(const std::string& computePath) : computePath(computePath) {
    // preprocessor.setDebugEnabled(true);
    // Preprocess shader to handle includes
    std::string computeCode = preprocessor.preprocessShader(computePath);

    //std::cout << "Preprocessed Compute Shader Code:\n" << computeCode << std::endl;

    const char* cShaderCode = computeCode.c_str();

    // Compile shaders
    unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, nullptr);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Delete shader
    glDeleteShader(compute);
}

void ComputeShader::use() {
    //std::cout << "Using " << "/ "<< computePath << " / shader\n";
    glUseProgram(ID);
    CheckGLError("Use Program");
}

void ComputeShader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    CheckGLError("ComputeShader::setBool");
}

void ComputeShader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    CheckGLError("ComputeShader::setInt");
}

void ComputeShader::setUInt(const std::string& name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    CheckGLError("ComputeShader::setUInt");
}

void ComputeShader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    CheckGLError("ComputeShader::setFloat");
}

void ComputeShader::setBVec2(const std::string& name, const glm::bvec2& value) const {
    glUniform2i(glGetUniformLocation(ID, name.c_str()), (int)value[0], (int)value[1]);
    CheckGLError("ComputeShader::setBVec2");
}

void ComputeShader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    CheckGLError("ComputeShader::setVec2");
}

void ComputeShader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    CheckGLError("ComputeShader::setVec2");
}

void ComputeShader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    CheckGLError("ComputeShader::setVec3");
}

void ComputeShader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    CheckGLError("ComputeShader::setVec3");
}

void ComputeShader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    CheckGLError("ComputeShader::setVec4");
}

void ComputeShader::setVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    CheckGLError("ComputeShader::setVec4");
}

void ComputeShader::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    CheckGLError("ComputeShader::setMat2");
}

void ComputeShader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    CheckGLError("ComputeShader::setMat3");
}

void ComputeShader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    CheckGLError("ComputeShader::setMat4");
}

void ComputeShader::DispatchComputeShader(GLuint particleCount, int numThreads) const {
    if (particleCount == 0 || numThreads <= 0) {
        std::cerr << "Invalid particle count or number of threads." << std::endl;
        return;
    }

    GLsizei numGroups = (particleCount + numThreads - 1) / numThreads;
    if (numGroups <= 0) {
        std::cerr << "Invalid number of work groups." << std::endl;
        return;
    }

    CheckGLError("Before DispatchCompute");

    glDispatchCompute(numGroups, 1, 1);
    CheckGLError("ComputeShader::DispatchComputeShader - DispatchCompute");

    glFinish();
    CheckGLError("ComputeShader::DispatchComputeShader - glFinish");

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    CheckGLError("ComputeShader::DispatchComputeShader - MemoryBarrier");
}


void ComputeShader::QueryMaxWorkGroupAndComputeUnits(GLint* maxWorkGroupCount, GLint* maxWorkGroupSize, GLint& maxComputeWorkGroupInvocations) const {
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroupCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxComputeWorkGroupInvocations);

    // std::cout << "Max work group count: (" << maxWorkGroupCount[0] << ", " << maxWorkGroupCount[1] << ", " << maxWorkGroupCount[2] << ")\n";
    // std::cout << "Max work group size: (" << maxWorkGroupSize[0] << ", " << maxWorkGroupSize[1] << ", " << maxWorkGroupSize[2] << ")\n";
    // std::cout << "Max compute work group invocations: " << maxComputeWorkGroupInvocations << "\n";
}

void ComputeShader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ComputeShader::checkCompileErrors ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ComputeShader::checkCompileErrors ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void ComputeShader::CheckGLError(const std::string& operation) const {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "ComputeShader::CheckGLError OpenGL error during " << operation << ": " << std::hex << err << std::dec << std::endl;
    }
}