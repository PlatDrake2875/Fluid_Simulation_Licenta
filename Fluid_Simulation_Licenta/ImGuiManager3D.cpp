#include "ImGuiManager3D.h"
#include "Simulation3D.h"
#include "ShaderManager3D.h"

// Cod adaptat de pe https://github.com/ocornut/imgui
ImGuiManager3D::ImGuiManager3D(Simulation3D* simulation, ShaderManager3D* shaderManager)
    : simulation(simulation), shaderManager(shaderManager) {}

ImGuiManager3D::~ImGuiManager3D() {
    Cleanup();
}

void ImGuiManager3D::Init() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Initialize ImGui for OpenGL
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();
}

void ImGuiManager3D::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager3D::Render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float offsetY = 0.0f;

    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(windowWidth) * 3 / 4, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth) / 4, static_cast<float>(windowHeight) * 1 / 4));

    ImGui::Begin("Debugging Information", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (simulation->getAppState() == AppState::MENU) {
        RenderMenu();
    }
    else if (simulation->getAppState() == AppState::SIMULATION) {
        RenderSimulationControls();
        RenderShaderManagerControls();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager3D::RenderMenu() {
    ImGui::Text("Select a Simulation:");
    if (ImGui::Button("Start Simulation")) {
        simulation->setAppState(AppState::SIMULATION);
    }
}

void ImGuiManager3D::RenderSimulationControls() {
    ImGui::Text("Simulation controls go here.");
    float timeScale = simulation->getTimeScale();
    bool isPaused = simulation->getIsPaused();
    ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 8.0f);
    ImGui::Checkbox("Pause", &isPaused);
    if (ImGui::Button("Restart Simulation")) {
        simulation->RestartSimulation();
    }
    simulation->setTimeScale(timeScale);
    simulation->setIsPaused(isPaused);

    RenderFPS();
}

void ImGuiManager3D::RenderFPS() {
    float frameTime = simulation->getFrameTime();
    float fps = 1.0f / frameTime;
    ImGui::Text("FPS: %.1f", fps);
}

void ImGuiManager3D::RenderShaderManagerControls() {
    shaderManager->RenderImGui();
}
