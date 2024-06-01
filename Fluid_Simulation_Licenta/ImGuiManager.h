#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glut.h"

class Simulation;
class ShaderManager;

class ImGuiManager {
public:
    ImGuiManager(Simulation* simulation, ShaderManager* shaderManager);
    ~ImGuiManager();

    void Init();
    void Cleanup();
    void Render();

private:
    Simulation* simulation;
    ShaderManager* shaderManager;

    void RenderSimulationControls();
    void RenderShaderManagerControls();
    void RenderWindow(const char* title, void (ImGuiManager::* renderFunc)(), float& offsetY);
    void RenderFPS();
    void RenderMenu();

    friend class Simulation;
};

#endif // IMGUIMANAGER_H
