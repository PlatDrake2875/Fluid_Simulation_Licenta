#ifndef SHADER_PREPROCESSOR_H
#define SHADER_PREPROCESSOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_map>

class ShaderPreprocessor {
public:
    ShaderPreprocessor() : debugEnabled(false) {}

    std::string preprocessShader(const std::string& filePath);

    void displayShaderCode(const std::string& filePath);

    void setDebugEnabled(bool enabled);

private:
    std::unordered_map<std::string, std::string> cache;
    bool debugEnabled;

    std::string preprocessShader(const std::string& filePath, std::unordered_map<std::string, std::string>& cache);
    std::string readFile(const std::string& filePath);
    void debugPrint(const std::string& message) const;
};

#endif // SHADER_PREPROCESSOR_H
