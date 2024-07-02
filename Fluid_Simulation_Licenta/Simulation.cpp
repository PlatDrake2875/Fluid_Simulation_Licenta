#include "Simulation.h"

Simulation* Simulation::instance = nullptr;
bool Simulation::resetSimulationFlag = false;

Simulation::Simulation(int argc, char** argv) {
    Initialize(argc, argv);
    instance = this;
}

void Simulation::Initialize(int argc, char** argv) {
    InitializeGLUT(argc, argv);
    InitializeGLEW();
    InitializeManagers();
}

void Simulation::InitializeGLUT(int argc, char** argv) {
    GlutInitializer glutInitializer(argc, argv);
    /*if (!glutInitializer.Initialize()) {
        std::cerr << "Failed to initialize GLUT\n";
        std::exit(EXIT_FAILURE);
    }*/

    // Initialize the main simulation window
    glutInitializer.InitializeWindow("SPH 2D", 1366, 768, 10, 10);
    mainWindowId = glutGetWindow();
    debugPrint("Main window ID: " + std::to_string(mainWindowId));
}

void Simulation::InitializeGLEW() {
    GlewInitializer glewInitializer;
    if (!glewInitializer.Initialize()) {
        std::cerr << "Failed to initialize GLEW\n";
        std::exit(EXIT_FAILURE);
    }
}

void Simulation::InitializeManagers() {
    shaderManager = new ShaderManager();
    shaderManager->SetupShaders();
    particleSystem = new ParticleSystem(shaderManager);

    imguiManager = new ImGuiManager(this, shaderManager);
    imguiManager->Init();
    debugPrint("ImGui manager initialized.");
}

Simulation::~Simulation() {
    Cleanup();
}

void Simulation::Cleanup() {
    imguiManager->Cleanup();
    delete imguiManager;
    delete shaderManager;
    delete particleSystem;
}

void Simulation::RestartSimulation() {
    delete particleSystem;
    particleSystem = new ParticleSystem(shaderManager);
    resetSimulationFlag = false;  // Reset the flag
}

void Simulation::Run() {
    debugPrint("Setting up callbacks for main window.");
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutTimerFunc(8, TimerCallback, 0);

    debugPrint("Entering GLUT main loop.");
    glutMainLoop();
}

void Simulation::Display() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, width * 3 / 4, height);

    if (appState == AppState::SIMULATION) {
        RunSimulationFrame(frameTime);
        particleSystem->DrawParticles();
        shaderManager->DrawBoundingBoxEdges();
    }

    glViewport(width * 3 / 4, 0, width / 4, height);
    ImGuiDisplay();

    glutSwapBuffers();
    glFlush();
}

void Simulation::DisplayCallback() {
    if (instance) {
        instance->Display();
    }
}

void Simulation::TimerCallback(int value) {
    if (instance) {
        instance->UpdateParticles();
        glutPostRedisplay();
        glutTimerFunc(16, TimerCallback, 0);
    }
}

void Simulation::ReshapeCallback(int width, int height) {
    glViewport(0, 0, width, height);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    if (instance) {
        instance->debugPrint("Window reshape: " + std::to_string(width) + "x" + std::to_string(height));
    }
    glutPostRedisplay();
}

void Simulation::ImGuiDisplay() {
    imguiManager->Render();
}

void Simulation::UpdateParticles() {
    particleSystem->UpdateParticles();
}

void Simulation::RunSimulationFrame(float frameTime) {
    if (resetSimulationFlag) {
        RestartSimulation();
    }
    if (!isPaused) {
        float timeStep = frameTime / iterationsPerFrame * timeScale;
        UpdateSettings(timeStep);

        for (int i = 0; i < iterationsPerFrame; i++) {
            particleSystem->UpdateParticles();
        }
    }
}


void Simulation::UpdateSettings(float timeStep) {
    shaderManager->UpdateComputeShaderSettings(timeStep);
}

void Simulation::DisplayCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime;
    localtime_s(&localTime, &currentTime);

    std::cout << "Current Time: " << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << '\n';
}

void Simulation::debugPrint(const std::string& message) {
    if (debugMode) {
        std::cout << message << std::endl;
    }
}
