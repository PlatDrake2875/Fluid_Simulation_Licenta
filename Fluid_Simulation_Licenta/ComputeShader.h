#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include "ShaderPreprocessor.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader {
public:
    unsigned int ID;
    /// <summary>
    /// Constructor that generates the compute shader
    /// </summary>
    /// <param name="computePath"></param>
    ComputeShader(const std::string& computePath);

    // activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setUInt(const std::string& name, unsigned int value) const;
    void setFloat(const std::string& name, float value) const;
    void setBVec2(const std::string& name, const glm::bvec2& value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    // Dispatch the compute shader
    void DispatchComputeShader(GLuint particleCount, int
        = 64) const;

private:
    ShaderPreprocessor preprocessor;

    void QueryMaxWorkGroupAndComputeUnits(GLint* maxWorkGroupCount, GLint* maxWorkGroupSize, GLint& maxComputeWorkGroupInvocations) const;

    void checkCompileErrors(GLuint shader, std::string type);
    void CheckGLError(const std::string& operation) const;
};

#endif // COMPUTE_SHADER_H
