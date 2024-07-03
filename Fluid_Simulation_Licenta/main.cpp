#include "SimulationFactory.h"
#include "SimType.h"
#include <GL/freeglut.h>
#include <iostream>

void* currentSimulation = nullptr;
SimType currentType = SimType::SIMULATION_2D;
int argc_global = 0;
char** argv_global = nullptr;

void KillCurrentSimulation() {
    switch (currentType) {
    case SimType::SIMULATION_2D:
        delete static_cast<Simulation*>(currentSimulation);
        break;
    case SimType::SIMULATION_3D:
        delete static_cast<Simulation3D*>(currentSimulation);
        break;
    }
    std::cout << "Killed Simulation\n";
    currentSimulation = nullptr;
}

void RunCurrentSimulation() {
    switch (currentType) {
    case SimType::SIMULATION_2D:
        currentSimulation = SimulationFactory::CreateSimulation(currentType, argc_global, argv_global);
        static_cast<Simulation*>(currentSimulation)->Run();
        break;
    case SimType::SIMULATION_3D:
        currentSimulation = SimulationFactory::CreateSimulation(currentType, argc_global, argv_global);
        static_cast<Simulation3D*>(currentSimulation)->Run();
        break;
    }
}

void KeyboardFunc(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        KillCurrentSimulation();
        currentType = (currentType == SimType::SIMULATION_2D) ? SimType::SIMULATION_3D : SimType::SIMULATION_2D;
        RunCurrentSimulation();
    }
}

void Initialize(int argc, char** argv) {
    argc_global = argc;
    argv_global = argv;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutKeyboardFunc(KeyboardFunc);
}

int main(int argc, char** argv) {
    std::cout << "Starting Fluid Simulation App..." << std::endl;

    Initialize(argc, argv);
    RunCurrentSimulation();

    glutMainLoop();

    KillCurrentSimulation();

    std::cout << "Exiting Fluid Simulation App..." << std::endl;
    return 0;
}
