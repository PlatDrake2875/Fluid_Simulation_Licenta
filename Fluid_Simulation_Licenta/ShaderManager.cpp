#include "ShaderManager.h"

ShaderManager::ShaderManager() : projection(glm::mat4(0.0f)), shader(nullptr), computeShader(nullptr) {}

ShaderManager::~ShaderManager() {
    delete shader;
    delete computeShader;
}

void ShaderManager::SetupShaders() {
    int width = 1024;
    int height = 768;

    SetupGraphicsShader(width, height);
    SetupComputeShader(currentComputeShader);
}

void ShaderManager::SetupGraphicsShader(int width, int height) {
    shader = new Shader("shaders/2D.vert", "shaders/2D.frag", "");
    glViewport(0, 0, width, height);
    projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    shader->use();
    shader->setMat4("projection", projection);
    shader->setFloat("maxSpeed", 50.0f);
}

void ShaderManager::SetupComputeShader(const std::string& shaderFile) {
    if (computeShader) {
        Simulation::resetSimulationFlag = true;
        delete computeShader;
    }
    computeShader = new ComputeShader("shaders/" + shaderFile);
    computeShader->use();

    ApplyComputeShaderSettings();
}

void ShaderManager::ApplyComputeShaderSettings() {
    computeShader->setUInt("numParticles", 10000); // Placeholder
    computeShader->setFloat("gravity", gravity);
    computeShader->setFloat("deltaTime", 0.01f);
    computeShader->setFloat("collisionDamping", collisionDamping);
    computeShader->setFloat("smoothingRadius", smoothingRadius);
    computeShader->setFloat("targetDensity", targetDensity);
    computeShader->setFloat("pressureMultiplier", pressureMultiplier);
    computeShader->setFloat("nearPressureMultiplier", nearPressureMultiplier);
    computeShader->setFloat("viscosityStrength", viscosityStrength);
    computeShader->setVec2("boundsSize", glm::vec2(1024.0f, 768.0f));
    computeShader->setVec4("boundingBox", boundingBox);
    computeShader->setVec2("interactionInputPoint", interactionInputPoint);
    computeShader->setFloat("interactionInputStrength", interactionInputStrength);
    computeShader->setFloat("interactionInputRadius", interactionInputRadius);
    computeShader->setFloat("Poly6ScalingFactor", 315.0f / (64.0f * static_cast<float>(M_PI) * powf(1.0f, 9.0f)));
    computeShader->setFloat("SpikyPow3ScalingFactor", 15.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
    computeShader->setFloat("SpikyPow2ScalingFactor", 45.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
    computeShader->setFloat("SpikyPow3DerivativeScalingFactor", -45.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
    computeShader->setFloat("SpikyPow2DerivativeScalingFactor", -135.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));

    computeShader->setBool("debugEnabled", false);
}

Shader* ShaderManager::GetShader() const {
    return shader;
}

ComputeShader* ShaderManager::GetComputeShader() const {
    return computeShader;
}

void ShaderManager::UpdateComputeShaderSettings(float timeStep) {
    computeShader->use();
    computeShader->setFloat("deltaTime", timeStep);
}

void ShaderManager::RenderImGui() {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowWidth) * 3 / 4, static_cast<float>(windowHeight) * 1 / 4));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth) / 4, static_cast<float>(windowHeight) * 3 / 4), ImGuiCond_Always);
    ImGui::Begin("Shader Manager", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    static const char* shaderOptions[] = { "SPH (slow)", "SPH (hashing)" };
    static int currentShaderIndex = 0;

    if (ImGui::Combo("Compute Shader", &currentShaderIndex, shaderOptions, IM_ARRAYSIZE(shaderOptions))) {
        const char* selectedShader = (currentShaderIndex == 0) ? "FluidSimulator.comp" : "FluidSimulatorHash.comp";
        SetupComputeShader(selectedShader);
    }

    RenderComputeShaderControls();

    ImGui::End();
}

void ShaderManager::SetInteractionInputPoint(const glm::vec2& point) {
    interactionInputPoint = point;
    computeShader->use();
    computeShader->setVec2("interactionInputPoint", interactionInputPoint);
}

void ShaderManager::UpdateMouseStateAndSetUniforms() {
    ImGuiIO& io = ImGui::GetIO();
    glm::bvec2 isXButtonDown = glm::bvec2(false, false);

    if (io.MouseDown[0]) {
        ImVec2 mousePos = ImGui::GetMousePos();
        SetInteractionInputPoint(glm::vec2(mousePos.x, mousePos.y));
        isXButtonDown = glm::bvec2(true, false);
    }
    else if (io.MouseDown[1]) {
        ImVec2 mousePos = ImGui::GetMousePos();
        SetInteractionInputPoint(glm::vec2(mousePos.x, mousePos.y));
        isXButtonDown = glm::bvec2(false, true);
    }

    computeShader->use();
    computeShader->setBVec2("isXButtonDown", isXButtonDown);
}

void ShaderManager::RenderComputeShaderControls() {
    ImGui::SliderFloat("Gravity", &gravity, 0.0f, 20.0f);
    ImGui::SliderFloat("Collision Damping", &collisionDamping, 0.0f, 1.0f);
    ImGui::SliderFloat("Smoothing Radius", &smoothingRadius, 0.0f, 10.0f);
    ImGui::SliderFloat("Target Density", &targetDensity, 0.0f, 10.0f);
    ImGui::SliderFloat("Pressure Multiplier", &pressureMultiplier, 0.0f, 10.0f);
    ImGui::SliderFloat("Near Pressure Multiplier", &nearPressureMultiplier, 0.0f, 10.0f);
    ImGui::SliderFloat("Viscosity Strength", &viscosityStrength, 0.0f, 10.0f);
    ImGui::SliderFloat("Interaction Input Strength", &interactionInputStrength, 0.0f, 30.0f);
    ImGui::SliderFloat("Interaction Input Radius", &interactionInputRadius, 0.0f, 300.0f);

    if (ImGui::SliderFloat4("Bounding Box", glm::value_ptr(boundingBox), 0.0f, 2000.0f)) {
        // Ensure that xMin < xMax and yMin < yMax
        if (boundingBox.x > boundingBox.z) boundingBox.x = boundingBox.z;
        if (boundingBox.y > boundingBox.w) boundingBox.y = boundingBox.w;
    }

    ImGui::Text("Interaction Input Point: (%.1f, %.1f)", interactionInputPoint.x, interactionInputPoint.y);

    UpdateMouseStateAndSetUniforms();

    if (ImGui::Button("Apply Changes")) {
        computeShader->use();
        ApplyComputeShaderSettings();
    }
}

void ShaderManager::DrawBoundingBoxEdges() {
    shader->use();

    glBegin(GL_LINE_LOOP);

    glVertex2f(boundingBox.x, boundingBox.y); // Bottom-left corner
    glVertex2f(boundingBox.z, boundingBox.y); // Bottom-right corner
    glVertex2f(boundingBox.z, boundingBox.w); // Top-right corner
    glVertex2f(boundingBox.x, boundingBox.w); // Top-left corner        

    glEnd();
    DrawInteractionRadiusCircle();
}

void ShaderManager::DrawInteractionRadiusCircle() {
    shader->use();

    const int numSegments = 100;
    const float angleIncrement = 2.0f * static_cast<float>(M_PI) / numSegments;

    glBegin(GL_LINE_LOOP);


    for (int i = 0; i < numSegments; ++i) {
        float angle = i * angleIncrement;
        float x = interactionInputPoint.x + interactionInputRadius * cos(angle);
        float y = interactionInputPoint.y + interactionInputRadius * sin(angle);
        glVertex2f(x, y);
    }

    glVertex2f(interactionInputPoint.x + interactionInputRadius, interactionInputPoint.y);

    glEnd();
}
