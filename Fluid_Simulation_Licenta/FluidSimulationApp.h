#ifndef FLUID_SIMULATION_APP_H
#define FLUID_SIMULATION_APP_H

// Compatibility stuff
#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <corecrt_math_defines.h>
#include "Shader.h"
#include "ComputeShader.h"
#include "ParticleGenerator.h"
#include "ParticleRenderer.h"
#include "GlutInitializer.h"
#include "GlewInitializer.h"

class FluidSimulationApp {
public:
    FluidSimulationApp(int argc, char** argv);
    ~FluidSimulationApp();
    void Run();
    void RunSimulationFrame(float frameTime);

private:
    Shader* shader;
    ComputeShader* computeShader;
    ParticleGenerator* particleGenerator;
    ParticleRenderer* particleRenderer;
    glm::mat4 projection;

    bool isPaused = false;
    float timeScale = 1.0f;
    int iterationsPerFrame = 1;

    void SetupShaderAndProjection();
    void InitParticleGenerator();
    void InitComputeShader();
    void UpdateParticles();
    void Display();
    void DisplayCurrentTime();
    void UpdateSettings(float timeStep);
    void RunSimulationStep();
    static FluidSimulationApp* instance;

    static void DisplayCallback();
    static void TimerCallback(int value);

};

#endif // FLUID_SIMULATION_APP_H
