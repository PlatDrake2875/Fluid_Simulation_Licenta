#include "ImGuiManager.h"
#include "Simulation.h"
#include "ShaderManager.h"

ImGuiManager::ImGuiManager(Simulation* simulation, ShaderManager* shaderManager)
    : simulation(simulation), shaderManager(shaderManager) {}

ImGuiManager::~ImGuiManager() {
    Cleanup();
}

void ImGuiManager::Init() {
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

void ImGuiManager::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::Render() {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    // Define the layout
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float offsetY = 0.0f;

    // Set the position and size of the main ImGui window that will hold the others
    ImGui::SetNextWindowPos(ImVec2(windowWidth * 3 / 4, 0));
    ImGui::SetNextWindowSize(ImVec2(windowWidth / 4, windowHeight * 3 / 4));
    ImGui::Begin("Debugging Information", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Render individual ImGui windows one below the other
    if (simulation->getAppState() == AppState::MENU) {
        RenderMenu();
    }
    else if (simulation->getAppState() == AppState::SIMULATION) {
        RenderSimulationControls();
        RenderShaderManagerControls();
    }

    ImGui::End();

    // Render ImGui into the screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::RenderMenu() {
    ImGui::Text("Select a Simulation:");
    if (ImGui::Button("Start Simulation")) {
        simulation->setAppState(AppState::SIMULATION);
    }
}

void ImGuiManager::RenderSimulationControls() {
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

void ImGuiManager::RenderFPS() {
    float frameTime = simulation->getFrameTime();
    float fps = 1.0f / frameTime;
    ImGui::Text("FPS: %.1f", fps);
}

void ImGuiManager::RenderShaderManagerControls() {
    shaderManager->RenderImGui();
}
