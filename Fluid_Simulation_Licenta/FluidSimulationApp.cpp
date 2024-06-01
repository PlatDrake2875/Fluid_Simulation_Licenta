#include "FluidSimulationApp.h"

FluidSimulationApp* FluidSimulationApp::instance = nullptr;
float xMin = -450.f, xMax = 450.f, yMin = -300.f, yMax = 300.f;

FluidSimulationApp::FluidSimulationApp(int argc, char** argv)
    : shader(nullptr), computeShader(nullptr), particleGenerator(nullptr), particleRenderer(nullptr)
{
    GlutInitializer glutInitializer(argc, argv);
    if (!glutInitializer.Initialize()) {
        std::cerr << "Failed to initialize GLUT\n";
        std::exit(EXIT_FAILURE);
    }
    glutInitializer.InitializeWindow("Particle Generator Example", 1024, 768, 100, 100);

    GlewInitializer glewInitializer;
    if (!glewInitializer.Initialize()) {
        std::cerr << "Failed to initialize GLEW\n";
        std::exit(EXIT_FAILURE);
    }

    SetupShaderAndProjection();
    InitParticleGenerator();
    InitComputeShader();

    ParticleGenerator::ParticleSpawnData spawnData = particleGenerator->GetSpawnData();
    particleRenderer = new ParticleRenderer(particleGenerator->GetParticleCount(), shader, computeShader, spawnData);

    instance = this;
}

FluidSimulationApp::~FluidSimulationApp() {
    delete shader;
    delete computeShader;
    delete particleGenerator;
    delete particleRenderer;
}

void FluidSimulationApp::Run() {
    glutDisplayFunc(DisplayCallback);
    glutTimerFunc(16, TimerCallback, 0);
    glutMainLoop();
}

void FluidSimulationApp::SetupShaderAndProjection() {
    int width = 1024;
    int height = 768;

    std::cout << "Setting up shaders and projection..." << std::endl;
    shader = new Shader("shaders/example.vert", "shaders/example.frag", "");

    glViewport(0, 0, width, height);

    projection = glm::ortho(0.0f, 0.5f * static_cast<float>(width), 0.5f * static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    shader->use();
    shader->setMat4("projection", projection);
    shader->setFloat("maxSpeed", 50.0f);
    std::cout << "Shaders and projection set up." << std::endl;
}

void FluidSimulationApp::InitParticleGenerator() {
    float screenWidth = 1024.0f;
    float screenHeight = 300.0f;

    std::cout << "Initializing particle generator..." << std::endl;
    particleGenerator = new ParticleGenerator(
        10000,                                // Number of particles
        glm::vec2(10.0f, 10.0f),                // Initial velocity
        glm::vec2(screenWidth / 2, screenHeight / 2),  // Spawn center
        glm::vec2(screenWidth, screenHeight),  // Spawn size
        0.5f                                  // Jitter strength
    );

    std::cout << "Particle generator initialized." << std::endl;
}

void FluidSimulationApp::InitComputeShader() {
    computeShader = new ComputeShader("shaders/FluidSimulator.comp");
    computeShader->use();

    computeShader->setUInt("numParticles", particleGenerator->GetParticleCount());
    computeShader->setFloat("gravity", 9.81f);
    computeShader->setFloat("deltaTime", 0.01f);
    computeShader->setFloat("collisionDamping", 0.9f);
    computeShader->setFloat("smoothingRadius", 1.0f);
    computeShader->setFloat("targetDensity", 1.0f);
    computeShader->setFloat("pressureMultiplier", 1.0f);
    computeShader->setFloat("nearPressureMultiplier", 1.0f);
    computeShader->setFloat("viscosityStrength", 0.1f);
    computeShader->setVec2("boundsSize", glm::vec2(1024.0f, 768.0f));
    computeShader->setVec2("interactionInputPoint", glm::vec2(1024.0f, 768.0f));
    computeShader->setFloat("interactionInputStrength", 1.0f);
    computeShader->setFloat("interactionInputRadius", 50.0f);
    computeShader->setFloat("Poly6ScalingFactor", 315.0f / (64.0f * static_cast<float>(M_PI) * powf(1.0f, 9.0f)));
    computeShader->setFloat("SpikyPow3ScalingFactor", 15.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
    computeShader->setFloat("SpikyPow2ScalingFactor", -45.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
    computeShader->setFloat("SpikyPow3DerivativeScalingFactor", -15.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
    computeShader->setFloat("SpikyPow2DerivativeScalingFactor", -45.0f / (static_cast<float>(M_PI) * powf(1.0f, 6.0f)));
}

void FluidSimulationApp::UpdateParticles() {
    particleRenderer->UpdateParticles();
}

void FluidSimulationApp::RunSimulationFrame(float frameTime) {
    if (!isPaused) {
        float timeStep = frameTime / iterationsPerFrame * timeScale;

        UpdateSettings(timeStep);

        for (int i = 0; i < iterationsPerFrame; i++) {
            RunSimulationStep();
        }
    }
}

void FluidSimulationApp::UpdateSettings(float timeStep) {
    computeShader->use();
    computeShader->setFloat("deltaTime", timeStep);
}

void FluidSimulationApp::RunSimulationStep() {
    UpdateParticles();
}

void FluidSimulationApp::DisplayCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime;
    localtime_s(&localTime, &currentTime);

    std::cout << "Current Time: " << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << '\n';
}

void FluidSimulationApp::Display() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RunSimulationFrame(0.016f); // Assuming 60 FPS, each frame is ~16ms

    particleRenderer->DrawParticles();

    // DisplayCurrentTime();

    glutSwapBuffers();
    glFlush();
}

void FluidSimulationApp::DisplayCallback() {
    if (instance) {
        instance->Display();
    }
}

void FluidSimulationApp::TimerCallback(int value) {
    if (instance) {
        instance->UpdateParticles();
        glutPostRedisplay();
        glutTimerFunc(16, TimerCallback, 0); // Call this function again in ~16ms (for ~60 FPS)
    }
}
