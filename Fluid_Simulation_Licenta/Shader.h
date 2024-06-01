#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Shader
{
public:
    unsigned int ID;

    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    int getUniformLocation(const std::string& name) const;

private:
    void checkCompileErrors(GLuint shader, const std::string& type) const;

    std::string readShaderCode(const std::string& shaderPath) const;
    GLuint compileShader(const std::string& shaderCode, GLenum shaderType) const;
    void CheckGLError(const std::string& operation) const;
};

#endif // SHADER_H
