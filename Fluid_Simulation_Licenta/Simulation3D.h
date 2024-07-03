#ifndef SIMULATION3D_H
#define SIMULATION3D_H
#include "ShaderManager3D.h"
#include "ParticleSystem3D.h"
#include "ImGuiManager3D.h"
#include "GlutInitializer.h"
#include "GlewInitializer.h"
#include "Movement.h"
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
#include "SceneBuilder.h"

class ShaderManager3D;
class ParticleSystem3D;
class ImGuiManager3D;
class Movement;
class SceneBuilder;

class Simulation3D {
public:
    Simulation3D(int argc, char** argv);
    ~Simulation3D();

    void Run();
    void Display();
    void UpdateParticles();
    void RunSimulationFrame(float frameTime);
    void UpdateSettings(float timeStep);
    void RestartSimulation();

    static void DisplayCallback();
    static void TimerCallback(int value);
    static void ReshapeCallback(int width, int height);
    static void KeyPressCallback(unsigned char key, int x, int y);
    static void KeyReleaseCallback(unsigned char key, int x, int y);
    static void MouseCallback(int button, int state, int x, int y);
    static void MouseMotionCallback(int x, int y);
    static void PassiveMouseMotionCallback(int x, int y);
    static void MouseScrollCallback(int button, int dir, int x, int y);

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

    ShaderManager3D* shaderManager;
    ParticleSystem3D* particleSystem;
    ImGuiManager3D* imguiManager;
    SceneBuilder* sceneBuilder;

    bool isPaused = false;
    float timeScale = 1.0f;
    float frameTime = 0.016f;
    AppState appState = AppState::MENU;

    int iterationsPerFrame = 1;

    int mainWindowId;
    int windowID;
    static Simulation3D* instance;

    void debugPrint(const std::string& message);
    bool debugMode = true;
};

#endif // SIMULATION3D_H
