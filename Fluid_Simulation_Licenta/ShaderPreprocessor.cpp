#include "ShaderPreprocessor.h"

void ShaderPreprocessor::setDebugEnabled(bool enabled) {
    debugEnabled = enabled;
}

void ShaderPreprocessor::debugPrint(const std::string& message) const {
    if (debugEnabled) {
        std::cout << message << std::endl;
    }
}

std::string ShaderPreprocessor::readFile(const std::string& filePath) {
    debugPrint("Reading file: " + filePath);
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    debugPrint("Read file content: \n" + buffer.str());
    return buffer.str();
}

std::string ShaderPreprocessor::preprocessShader(const std::string& filePath) {
    debugPrint("Preprocessing shader: " + filePath);
    return preprocessShader(filePath, cache);
}

std::string ShaderPreprocessor::preprocessShader(const std::string& filePath, std::unordered_map<std::string, std::string>& cache) {
    debugPrint("Checking cache for: " + filePath);
    if (cache.find(filePath) != cache.end()) {
        debugPrint("Cache hit for: " + filePath);
        return cache[filePath];
    }

    debugPrint("Cache miss for: " + filePath);
    std::string shaderCode = readFile(filePath);
    std::regex includePattern(R"(#include\s*\"(.+?)\")");
    std::smatch matches;

    while (std::regex_search(shaderCode, matches, includePattern)) {
        std::string includeFilePath = matches[1].str();
        debugPrint("Found include directive for: " + includeFilePath);
        std::string includeFileContent = preprocessShader(includeFilePath, cache);
        debugPrint("Replacing include directive with content of: " + includeFilePath);
        shaderCode.replace(matches.position(0), matches.length(0), includeFileContent);
    }

    cache[filePath] = shaderCode;
    debugPrint("Caching preprocessed shader for: " + filePath);
    return shaderCode;
}

void ShaderPreprocessor::displayShaderCode(const std::string& filePath) {
    std::string finalCode = preprocessShader(filePath);
    std::cout << "Final preprocessed shader code for " + filePath + ":\n" + finalCode << std::endl;
}
