#ifndef IMGUIMANAGER3D_H
#define IMGUIMANAGER3D_H

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glut.h"

class Simulation3D;
class ShaderManager3D;

class ImGuiManager3D {
public:
    ImGuiManager3D(Simulation3D* simulation, ShaderManager3D* shaderManager);
    ~ImGuiManager3D();

    void Init();
    void Cleanup();
    void Render();

private:
    Simulation3D* simulation;
    ShaderManager3D* shaderManager;

    void RenderSimulationControls();
    void RenderShaderManagerControls();
    void RenderFPS();
    void RenderMenu();

    friend class Simulation3D;
};

#endif // IMGUIMANAGER3D_H
