#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "Shader.h"
#include "ComputeShader.h"
#include <glm/glm.hpp>
#include "imgui.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <corecrt_math_defines.h>
#include <glm/gtc/type_ptr.hpp>

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    void SetupShaders();
    Shader* GetShader() const;
    ComputeShader* GetComputeShader() const;
    void UpdateComputeShaderSettings(float timeStep);
    void RenderImGui();

private:
    Shader* shader;
    ComputeShader* computeShader;
    glm::mat4 projection;

    // Shader parameters
    float gravity = 9.81f;
    float collisionDamping = 0.9f;
    float smoothingRadius = 1.0f;
    float targetDensity = 1.0f;
    float pressureMultiplier = 1.0f;
    float nearPressureMultiplier = 1.0f;
    float viscosityStrength = 0.1f;
    float interactionInputStrength = 10.0f;
    float interactionInputRadius = 100.0f;
    glm::vec2 interactionInputPoint = glm::vec2(1024.0f, 768.0f);
    glm::vec4 boundingBox = glm::vec4(0.0f, 0.0f, 1024.0f, 768.0f);
    glm::bvec2 isXButtonDown = glm::bvec2(false, false);

    void SetupGraphicsShader(int width, int height);
    void SetupComputeShader();
    void ApplyComputeShaderSettings();
    void SetInteractionInputPoint(const glm::vec2& point);
    void UpdateMouseStateAndSetUniforms();
    void RenderComputeShaderControls();
    void DrawBoundingBoxEdges();

    void DrawInteractionRadiusCircle();

    friend class Simulation;
};

#endif // SHADERMANAGER_H
