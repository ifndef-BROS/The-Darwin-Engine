#include "simulation.h"
#include "parameters.h"
#include <iostream>

int main() {
    sim::Parameters params;
    
    // params.populationSize = 100;
    // params.maxGenerations = 200;
    // params.trainingEpisodesPerGen = 1000;

    try {
        sim::Simulation simulation(params);
        simulation.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
