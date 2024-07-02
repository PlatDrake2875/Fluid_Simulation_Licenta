// SimulationFactory.h
#ifndef SIMULATIONFACTORY_H
#define SIMULATIONFACTORY_H

#include "SimType.h"
#include "Simulation.h"
#include "Simulation3D.h"

class SimulationFactory {
public:
    static void* CreateSimulation(SimType type, int argc, char** argv) {
        switch (type) {
        case SimType::SIMULATION_2D:
            return new Simulation(argc, argv); 
        case SimType::SIMULATION_3D:
            return new Simulation3D(argc, argv);
        default:
            return nullptr;
        }
    }
};

#endif // SIMULATIONFACTORY_H
