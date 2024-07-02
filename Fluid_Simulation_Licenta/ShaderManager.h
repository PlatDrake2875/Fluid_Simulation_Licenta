#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "Simulation.h"
#include "Shader.h"
#include "ComputeShader.h"
#include <glm/glm.hpp>
#include "imgui.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <corecrt_math_defines.h>
#include <glm/gtc/type_ptr.hpp>
#include <GL/freeglut_std.h>

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    void SetupShaders();
    void SetupGraphicsShader(int width, int height);
    void SetupComputeShader(const std::string& shaderFile);
    void ApplyComputeShaderSettings();
    Shader* GetShader() const;
    ComputeShader* GetComputeShader() const;
    void UpdateComputeShaderSettings(float timeStep);
    void RenderImGui();
    void SetInteractionInputPoint(const glm::vec2& point);
    void UpdateMouseStateAndSetUniforms();
    void RenderComputeShaderControls();
    void DrawBoundingBoxEdges();
    void DrawInteractionRadiusCircle();

private:
    glm::mat4 projection;
    Shader* shader;
    ComputeShader* computeShader;
    float gravity = 9.8f;
    float collisionDamping = 0.5f;
    float smoothingRadius = 1.0f;
    float targetDensity = 1.0f;
    float pressureMultiplier = 1.0f;
    float nearPressureMultiplier = 1.0f;
    float viscosityStrength = 1.0f;
    glm::vec2 interactionInputPoint = glm::vec2(0.0f, 0.0f);
    float interactionInputStrength = 1.0f;
    float interactionInputRadius = 50.0f;
    glm::vec4 boundingBox = glm::vec4(0.0f, 0.0f, 1024.0f, 768.0f);
    std::string currentComputeShader = "FluidSimulator.comp";
};

#endif // SHADERMANAGER_H
