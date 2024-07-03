#ifndef SIMULATION_FACTORY_H
#define SIMULATION_FACTORY_H

#include "SimType.h"
#include "Simulation.h"
#include "Simulation3D.h"

class SimulationFactory {
public:
    static void* CreateSimulation(SimType type, int argc, char** argv);
};

#endif // SIMULATION_FACTORY_H
