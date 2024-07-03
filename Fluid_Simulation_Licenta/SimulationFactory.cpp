#include "SimulationFactory.h"

void* SimulationFactory::CreateSimulation(SimType type, int argc, char** argv) {
    switch (type) {
    case SimType::SIMULATION_2D:
        return new Simulation(argc, argv);
    case SimType::SIMULATION_3D:
        return new Simulation3D(argc, argv);
    default:
        return nullptr;
    }
}
