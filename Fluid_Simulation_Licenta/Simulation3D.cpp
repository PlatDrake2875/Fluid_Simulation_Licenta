#include "Simulation3D.h"

Simulation3D* Simulation3D::instance = nullptr;
bool Simulation3D::resetSimulationFlag = false;

Simulation3D::Simulation3D(int argc, char** argv) {
    Initialize(argc, argv);
    instance = this;
}

void Simulation3D::Initialize(int argc, char** argv) {
    InitializeGLUT(argc, argv);
    InitializeGLEW();
    InitializeManagers();
}

void Simulation3D::InitializeGLUT(int argc, char** argv) {
    GlutInitializer glutInitializer(argc, argv);
    glutInitializer.InitializeWindow("SPH 3D", 1366, 768, 10, 10);
    mainWindowId = glutGetWindow();
    debugPrint("Main window ID: " + std::to_string(mainWindowId));
}

void Simulation3D::InitializeGLEW() {
    GlewInitializer glewInitializer;
    if (!glewInitializer.Initialize()) {
        std::cerr << "Failed to initialize GLEW\n";
        std::exit(EXIT_FAILURE);
    }
}

void Simulation3D::InitializeManagers() {
    shaderManager = new ShaderManager3D();
    shaderManager->SetupShaders();
    particleSystem = new ParticleSystem3D(shaderManager);

    imguiManager = new ImGuiManager3D(this, shaderManager);
    imguiManager->Init();
    debugPrint("ImGui manager initialized.");

    sceneBuilder = new SceneBuilder(shaderManager);
}

Simulation3D::~Simulation3D() {
    Cleanup();
}

void Simulation3D::Cleanup() {
    imguiManager->Cleanup();
    delete imguiManager;
    delete shaderManager;
    delete particleSystem;
    delete sceneBuilder; 
}

void Simulation3D::RestartSimulation() {
    delete particleSystem;
    particleSystem = new ParticleSystem3D(shaderManager);
    resetSimulationFlag = false; 
}

void Simulation3D::Run() {
    debugPrint("Setting up callbacks for main window.");
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyPressCallback);
    glutKeyboardUpFunc(KeyReleaseCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MouseMotionCallback);
    glutPassiveMotionFunc(PassiveMouseMotionCallback);
    glutMouseWheelFunc(MouseScrollCallback);
    glutTimerFunc(16, TimerCallback, 0);

    debugPrint("Entering GLUT main loop.");
    glutMainLoop();
}

void Simulation3D::Display() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, width * 3 / 4, height);

    if (appState == AppState::SIMULATION) {
        RunSimulationFrame(frameTime);
        particleSystem->DrawParticles(shaderManager->GetCamera());
        sceneBuilder->renderMeshes();
    }

    glViewport(width * 3 / 4, 0, width / 4, height);
    ImGuiDisplay();

    glutSwapBuffers();
    glFlush();
}

void Simulation3D::DisplayCallback() {
    if (instance) {
        instance->Display();
    }
}

void Simulation3D::TimerCallback(int value) {
    if (instance) {
        instance->UpdateParticles();
        glutPostRedisplay();
        glutTimerFunc(16, TimerCallback, 0);
    }
}

void Simulation3D::ReshapeCallback(int width, int height) {
    glViewport(0, 0, width, height);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    if (instance) {
        instance->debugPrint("Window reshape: " + std::to_string(width) + "x" + std::to_string(height));
    }
    glutPostRedisplay();
}

void Simulation3D::KeyPressCallback(unsigned char key, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = true;
    io.AddInputCharacter(key);
    if (instance && instance->shaderManager->GetMovementHandler() && !io.WantCaptureKeyboard) {
        instance->shaderManager->GetMovementHandler()->keyPress(key);
    }
}

void Simulation3D::KeyReleaseCallback(unsigned char key, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;
    if (instance && instance->shaderManager->GetMovementHandler() && !io.WantCaptureKeyboard) {
        instance->shaderManager->GetMovementHandler()->keyRelease(key);
    }
}

void Simulation3D::MouseCallback(int button, int state, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown)) {
        io.MouseDown[button] = (state == GLUT_DOWN);
    }
    if (instance && instance->shaderManager->GetMovementHandler() && !io.WantCaptureMouse) {
        instance->shaderManager->GetMovementHandler()->mousePress(button, state, x, y); 
    }
}

void Simulation3D::MouseMotionCallback(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    if (instance && instance->shaderManager->GetMovementHandler() && !io.WantCaptureMouse) {
        instance->shaderManager->GetMovementHandler()->processMouseMovement(x, y);
    }
}

void Simulation3D::PassiveMouseMotionCallback(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
}

void Simulation3D::MouseScrollCallback(int button, int dir, int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    if (dir > 0)
        io.MouseWheel += 1;
    else if (dir < 0)
        io.MouseWheel -= 1;

    if (instance && instance->shaderManager->GetMovementHandler() && !io.WantCaptureMouse) {
        instance->shaderManager->GetMovementHandler()->processMouseScroll(button, dir, x, y);
    }
}

void Simulation3D::ImGuiDisplay() {
    imguiManager->Render();
}

void Simulation3D::UpdateParticles() {
    particleSystem->UpdateParticles();
    if (shaderManager->isBoundingBoxChanged()) {
        sceneBuilder->updateBoundingBox();
        shaderManager->resetBoundingBoxChanged();
    }
}

void Simulation3D::RunSimulationFrame(float frameTime) {
    if (resetSimulationFlag) {
        RestartSimulation();
    }
    if (!isPaused) {
        float smoothingRadius = shaderManager->GetSmoothingRadius();

        float maxVelocity = particleSystem->GetMaxVelocity();
        float deltaTime = frameTime / iterationsPerFrame;
        float maxAcceleration = particleSystem->GetMaxAcceleration(deltaTime);

        if (maxVelocity == 0.0f) maxVelocity = 1.0f;
        if (maxAcceleration == 0.0f) maxAcceleration = 1.0f;

        float timeStepVelocity = 0.25f * (smoothingRadius / maxVelocity); // CFL condition for PDEs
        float timeStepAcceleration = 0.25f * sqrt(smoothingRadius / maxAcceleration); // Force-based condition

        float adaptiveTimeStep = std::min(timeStepVelocity, timeStepAcceleration);

        // Ensure timeStep is within a reasonable range
        float minTimeStep = 0.0001f;
        float maxTimeStep = 0.01f;
        adaptiveTimeStep = glm::clamp(adaptiveTimeStep, minTimeStep, maxTimeStep);

        float timeStep = adaptiveTimeStep * timeScale;

        shaderManager->UpdateComputeShaderSettings(timeStep);
        shaderManager->GetMovementHandler()->processInput(timeStep);

        for (int i = 0; i < iterationsPerFrame; i++) {
            particleSystem->UpdateParticles();
        }
    }
}

void Simulation3D::UpdateSettings(float timeStep) {
    shaderManager->UpdateComputeShaderSettings(timeStep);
}

void Simulation3D::DisplayCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime;
    localtime_s(&localTime, &currentTime);

    std::cout << "Current Time: " << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << '\n';
}

void Simulation3D::debugPrint(const std::string& message) {
    if (debugMode) {
        std::cout << message << std::endl;
    }
}
