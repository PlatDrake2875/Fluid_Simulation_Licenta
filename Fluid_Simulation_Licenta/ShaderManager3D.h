#ifndef SHADERMANAGER3D_H
#define SHADERMANAGER3D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Shader.h"
#include "ComputeShader.h"
#include "Simulation3D.h"
#include <corecrt_math_defines.h>
#include <imgui.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "Movement.h"

class ShaderManager3D {
public:
    ShaderManager3D();
    ~ShaderManager3D();

    void SetupShaders();
    void SetupGraphicsShader(int width, int height);
    void SetupComputeShader(const std::string& shaderFile);
    void ApplyComputeShaderSettings();
    void RenderImGui();
    void UpdateComputeShaderSettings(float timeStep);
    void DrawBoundingBoxEdges();
    void DrawInteractionRadiusSphere();

    Shader* GetShader() const;
    ComputeShader* GetComputeShader() const;
    Camera* GetCamera() const;
    float GetGravity() const { return gravity; }
    float GetCollisionDamping() const { return collisionDamping; }
    float GetSmoothingRadius() const { return smoothingRadius; }
    float GetTargetDensity() const { return targetDensity; }
    float GetPressureMultiplier() const { return pressureMultiplier; }
    float GetNearPressureMultiplier() const { return nearPressureMultiplier; }
    float GetViscosityStrength() const { return viscosityStrength; }
    Movement* GetMovementHandler() const { return movementHandler; }
    glm::vec3 GetBoundingBoxMin() const;
    glm::vec3 GetBoundingBoxMax() const;

    bool isBoundingBoxChanged() const { return boundingBoxChanged; }
    void resetBoundingBoxChanged() { boundingBoxChanged = false; }

    void SetInteractionInputPoint(const glm::vec3& point);
    void UpdateMouseStateAndSetUniforms();

private:
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    Shader* shader;
    ComputeShader* computeShader;
    Camera* camera;
    Movement* movementHandler;
    glm::vec3 interactionInputPoint;
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
    bool boundingBoxChanged = false;
    float gravity = 9.81f;
    float collisionDamping = 0.1f;
    float smoothingRadius = 1.0f;
    float targetDensity = 1.0f;
    float pressureMultiplier = 1.0f;
    float nearPressureMultiplier = 1.0f;
    float viscosityStrength = 1.0f;
    float interactionInputStrength = 1.0f;
    float interactionInputRadius = 1.0f;
    std::string currentComputeShader;

    void RenderComputeShaderControls();
};

#endif // SHADERMANAGER3D_H
