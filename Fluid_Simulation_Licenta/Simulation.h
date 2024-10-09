// Simulation.h
#ifndef SIMULATION_H
#define SIMULATION_H
#include "ShaderManager.h"
#include "ParticleSystem.h"
#include "ImGuiManager.h"
#include "GlutInitializer.h"
#include "GlewInitializer.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glut.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include "AppState.h"

class ShaderManager;  
class ParticleSystem;
class ImGuiManager;


class Simulation {
public:
    Simulation(int argc, char** argv);
    ~Simulation();

    void Run();
    void Display();
    void UpdateParticles();
    void RunSimulationFrame(float frameTime);
    void UpdateSettings(float timeStep);
    void RestartSimulation();

    static void DisplayCallback();
    static void TimerCallback(int value);
    static void ReshapeCallback(int width, int height);

    bool getIsPaused() const { return isPaused; }
    void setIsPaused(bool value) { isPaused = value; }
    float getTimeScale() const { return timeScale; }
    void setTimeScale(float value) { timeScale = value; }
    float getFrameTime() const { return frameTime; }
    void setFrameTime(float value) { frameTime = value; }
    AppState getAppState() const { return appState; }
    void setAppState(AppState state) { appState = state; }
    void setResetSimulationFlag(float value) { resetSimulationFlag = value; }

    static bool resetSimulationFlag;

    void SetWindowID(int id) { windowID = id; }

private:
    void Initialize(int argc, char** argv);
    void InitializeGLUT(int argc, char** argv);
    void InitializeGLEW();
    void InitializeManagers();
    void Cleanup();
    void DisplayCurrentTime();
    void ImGuiDisplay();

    ShaderManager* shaderManager;
    ParticleSystem* particleSystem;
    ImGuiManager* imguiManager;

    bool isPaused = false;
    float timeScale = 2.5f;
    float frameTime = 0.016f;
    AppState appState = AppState::MENU;

    int iterationsPerFrame = 1;

    int mainWindowId;
    int windowID;
    static Simulation* instance;

    void debugPrint(const std::string& message);
    bool debugMode = true;
};

#endif // SIMULATION_H
