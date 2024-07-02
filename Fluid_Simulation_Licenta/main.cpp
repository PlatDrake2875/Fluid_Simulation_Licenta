#include "SimulationFactory.h"
#include "SimType.h"
#include <GL/freeglut.h>
#include <iostream>

void* currentSimulation = nullptr;
SimType currentType = SimType::SIMULATION_3D;

void KillCurrentSimulation() {
    switch (currentType) {
    case SimType::SIMULATION_2D:
        delete static_cast<Simulation*>(currentSimulation);
        break;
    case SimType::SIMULATION_3D:
        delete static_cast<Simulation3D*>(currentSimulation);
        break;
    }
    currentSimulation = nullptr;
}

void RunCurrentSimulation(int argc, char** argv) {
    switch (currentType) {
    case SimType::SIMULATION_2D:
        currentSimulation = SimulationFactory::CreateSimulation(currentType, argc, argv);
        static_cast<Simulation*>(currentSimulation)->Run();
        break;
    case SimType::SIMULATION_3D:
        currentSimulation = SimulationFactory::CreateSimulation(currentType, argc, argv);
        static_cast<Simulation3D*>(currentSimulation)->Run();
        break;
    }
}

void KeyboardFunc(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        KillCurrentSimulation();
        currentType = (currentType == SimType::SIMULATION_2D) ? SimType::SIMULATION_3D : SimType::SIMULATION_2D;
        RunCurrentSimulation(0, nullptr);
    }
}

void Initialize(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutKeyboardFunc(KeyboardFunc);
}

int main(int argc, char** argv) {
    std::cout << "Starting Fluid Simulation App..." << std::endl;

    Initialize(argc, argv);
    RunCurrentSimulation(argc, argv);

    glutMainLoop();

    KillCurrentSimulation();

    std::cout << "Exiting Fluid Simulation App..." << std::endl;
    return 0;
}
