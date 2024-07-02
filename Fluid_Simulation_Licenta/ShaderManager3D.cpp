#include "ShaderManager3D.h"

ShaderManager3D::ShaderManager3D()
    : projection(glm::mat4(1.0f)),
    shader(nullptr),
    computeShader(nullptr),
    interactionInputPoint(glm::vec3(0.0f)),
    boundingBoxMin(glm::vec3(0.0f, 0.0f, 0.0f)),
    boundingBoxMax(glm::vec3(32.0f, 32.0f, 32.0f)),
    gravity(9.81f),
    collisionDamping(0.5f),
    smoothingRadius(1.0f),
    targetDensity(1.0f),
    pressureMultiplier(1.0f),
    nearPressureMultiplier(1.0f),
    viscosityStrength(1.0f),
    interactionInputStrength(1.0f),
    interactionInputRadius(1.0f),
    currentComputeShader("FluidSimulator_3D.comp"),
    camera(new Camera(glm::vec3(32.0f, 32.0f, 32.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 45.0f, 1.0f, 0.1f, 100.0f)),
    movementHandler(new Movement(*camera)) {

    camera->setDebugMode(true);
}

ShaderManager3D::~ShaderManager3D() {
    delete shader;
    delete computeShader;
    delete camera;
    delete movementHandler;
}

void ShaderManager3D::SetupShaders() {
    int width = 1024;
    int height = 768;

    SetupGraphicsShader(width, height);
    SetupComputeShader(currentComputeShader);
}

void ShaderManager3D::SetupGraphicsShader(int width, int height) {
    shader = new Shader("shaders/3D.vert", "shaders/3D.frag", "");
    glViewport(0, 0, width, height);

    shader->use();

    shader->setMat4("projection", camera->getProjectionMatrix());
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("model", camera->getModelMatrix());

    shader->setFloat("maxSpeed", 50.0f);
}


void ShaderManager3D::SetupComputeShader(const std::string& shaderFile) {
    if (computeShader) {
        Simulation3D::resetSimulationFlag = true;
        delete computeShader;
    }
    computeShader = new ComputeShader("shaders/" + shaderFile);
    computeShader->use();

    ApplyComputeShaderSettings();
}

void ShaderManager3D::ApplyComputeShaderSettings() {
    computeShader->setUInt("numParticles", 10000); // Placeholder
    computeShader->setFloat("gravity", gravity);
    computeShader->setFloat("deltaTime", 0.000001f);
    computeShader->setFloat("collisionDamping", collisionDamping);
    computeShader->setFloat("smoothingRadius", smoothingRadius);
    computeShader->setFloat("targetDensity", targetDensity);
    computeShader->setFloat("pressureMultiplier", pressureMultiplier);
    computeShader->setFloat("nearPressureMultiplier", nearPressureMultiplier);
    computeShader->setFloat("viscosityStrength", viscosityStrength);
    computeShader->setVec3("boundsSize", glm::vec3(124.0f, 124.0f, 124.0f));
    computeShader->setVec3("boundingBoxMin", boundingBoxMin);
    computeShader->setVec3("boundingBoxMax", boundingBoxMax);
    computeShader->setVec3("interactionInputPoint", interactionInputPoint);
    computeShader->setFloat("interactionInputStrength", interactionInputStrength);
    computeShader->setFloat("interactionInputRadius", interactionInputRadius);
    computeShader->setFloat("Poly6ScalingFactor", 315.0f / (64.0f * static_cast<float>(M_PI) * powf(smoothingRadius, 9.0f)));
    computeShader->setFloat("SpikyPow3ScalingFactor", 15.0f / (static_cast<float>(M_PI) * powf(smoothingRadius, 6.0f)));
    computeShader->setFloat("SpikyPow2ScalingFactor", -45.0f / (static_cast<float>(M_PI) * powf(smoothingRadius, 6.0f)));
    computeShader->setFloat("SpikyPow3DerivativeScalingFactor", -45.0f / (static_cast<float>(M_PI) * powf(smoothingRadius, 6.0f)));
    computeShader->setFloat("SpikyPow2DerivativeScalingFactor", -135.0f / (static_cast<float>(M_PI) * powf(smoothingRadius, 6.0f)));

    computeShader->setBool("debugEnabled", false);
}

Shader* ShaderManager3D::GetShader() const {
    return shader;
}

ComputeShader* ShaderManager3D::GetComputeShader() const {
    return computeShader;
}

Camera* ShaderManager3D::GetCamera() const{
    return camera;
}

glm::vec3 ShaderManager3D::GetBoundingBoxMin() const {
    return boundingBoxMin;
}

glm::vec3 ShaderManager3D::GetBoundingBoxMax() const {
    return boundingBoxMax;
}


void ShaderManager3D::UpdateComputeShaderSettings(float timeStep) {
    computeShader->use();
    computeShader->setFloat("deltaTime", timeStep);
}

void ShaderManager3D::RenderImGui() {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowWidth) * 3 / 4, static_cast<float>(windowHeight) * 1 / 4));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth) / 4, static_cast<float>(windowHeight) * 3 / 4), ImGuiCond_Always);
    ImGui::Begin("Shader Manager", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    static const char* shaderOptions[] = { "SPH (slow)", "SPH (hashing)" };
    static int currentShaderIndex = 0;

    if (ImGui::Combo("Compute Shader", &currentShaderIndex, shaderOptions, IM_ARRAYSIZE(shaderOptions))) {
        const char* selectedShader = (currentShaderIndex == 0) ? "FluidSimulator_3D.comp" : "FluidSimulatorHash_3D.comp";
        SetupComputeShader(selectedShader);
    }

    RenderComputeShaderControls();

    ImGui::End();
}

void ShaderManager3D::SetInteractionInputPoint(const glm::vec3& point) {
    interactionInputPoint = point;
    computeShader->use();
    computeShader->setVec3("interactionInputPoint", interactionInputPoint);
}

void ShaderManager3D::UpdateMouseStateAndSetUniforms() {
    ImGuiIO& io = ImGui::GetIO();
    glm::bvec2 isXButtonDown = glm::bvec2(false, false);

    if (io.MouseDown[0]) {
        ImVec2 mousePos = ImGui::GetMousePos();
        SetInteractionInputPoint(glm::vec3(mousePos.x, mousePos.y, 0.0f));
        isXButtonDown = glm::bvec2(true, false);
    }
    else if (io.MouseDown[1]) {
        ImVec2 mousePos = ImGui::GetMousePos();
        SetInteractionInputPoint(glm::vec3(mousePos.x, mousePos.y, 0.0f));
        isXButtonDown = glm::bvec2(false, true);
    }

    computeShader->use();
    computeShader->setBVec2("isXButtonDown", isXButtonDown);
}

void ShaderManager3D::RenderComputeShaderControls() {
    ImGui::SliderFloat("Gravity", &gravity, 0.0f, 20.0f);
    ImGui::SliderFloat("Collision Damping", &collisionDamping, 0.0f, 1.0f);
    ImGui::SliderFloat("Smoothing Radius", &smoothingRadius, 0.0f, 10.0f);
    ImGui::SliderFloat("Target Density", &targetDensity, 0.0f, 10.0f);
    ImGui::SliderFloat("Pressure Multiplier", &pressureMultiplier, 0.0f, 10.0f);
    ImGui::SliderFloat("Near Pressure Multiplier", &nearPressureMultiplier, 0.0f, 10.0f);
    ImGui::SliderFloat("Viscosity Strength", &viscosityStrength, 0.0f, 10.0f);
    ImGui::SliderFloat("Interaction Input Strength", &interactionInputStrength, 0.0f, 30.0f);
    ImGui::SliderFloat("Interaction Input Radius", &interactionInputRadius, 0.0f, 300.0f);

    if (ImGui::SliderFloat3("Bounding Box Min (xMin, yMin, zMin)", glm::value_ptr(boundingBoxMin), 0.0f, 2000.0f)) {
        boundingBoxMin = glm::min(boundingBoxMin, boundingBoxMax);
        boundingBoxChanged = true;
    }

    if (ImGui::SliderFloat3("Bounding Box Max (xMax, yMax, zMax)", glm::value_ptr(boundingBoxMax), 0.0f, 2000.0f)) {
        boundingBoxMax = glm::max(boundingBoxMax, boundingBoxMin);
        boundingBoxChanged = true;
    }
    ImGui::Text("Interaction Input Point: (%.1f, %.1f, %.1f)", interactionInputPoint.x, interactionInputPoint.y, interactionInputPoint.z);

    UpdateMouseStateAndSetUniforms();

    if (ImGui::Button("Apply Changes")) {
        computeShader->use();
        ApplyComputeShaderSettings();
    }
}

void ShaderManager3D::DrawBoundingBoxEdges() {
    shader->use();

    // Set the matrices from the camera
    shader->setMat4("projection", camera->getProjectionMatrix());
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("model", glm::mat4(1.0f)); // Assuming the model matrix is identity for the bounding box

    // Draw the bounding box edges using GL_LINES
    glBegin(GL_LINES);

    // Bottom edges
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z);

    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z);

    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z);

    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z);

    // Top edges
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z);

    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z);

    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z);

    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z);

    // Vertical edges
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z);

    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z);

    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z);

    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z);

    glEnd();
}


void ShaderManager3D::DrawInteractionRadiusSphere() {
    shader->use();

    const int numSegments = 36;  // Number of segments for latitude and longitude
    const float angleIncrement = 2.0f * static_cast<float>(M_PI) / numSegments;

    for (int i = 0; i <= numSegments; ++i) {
        float lat0 = static_cast<float>(M_PI) * (-0.5f + static_cast<float>(i - 1) / numSegments);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);

        float lat1 = static_cast<float>(M_PI) * (-0.5f + static_cast<float>(i) / numSegments);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= numSegments; ++j) {
            float lng = angleIncrement * static_cast<float>(j);
            float x = cos(lng);
            float y = sin(lng);

            glVertex3f(interactionInputPoint.x + interactionInputRadius * x * zr0,
                interactionInputPoint.y + interactionInputRadius * y * zr0,
                interactionInputPoint.z + interactionInputRadius * z0);

            glVertex3f(interactionInputPoint.x + interactionInputRadius * x * zr1,
                interactionInputPoint.y + interactionInputRadius * y * zr1,
                interactionInputPoint.z + interactionInputRadius * z1);
        }
        glEnd();
    }
}
