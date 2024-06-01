#include "Simulation.h"
#undef main

int main(int argc, char** argv) {
    std::cout << "Starting Fluid Simulation App..." << std::endl;
    Simulation simulation(argc, argv);
    simulation.Run();
    std::cout << "Exiting Fluid Simulation App..." << std::endl;
    return 0;
}
