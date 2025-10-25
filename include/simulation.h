#ifndef SIMULATION_H
#define SIMULATION_H

#include "indiv.h"
#include "grid.h"
#include "parameters.h"
#include "data_logger.h"
#include <vector>
#include <random>
#include <string>
#include <map>

namespace sim {

class Simulation {
public:
    Simulation(const Parameters& params);
    void run();

private:
    void initializePopulation();
    void spawnNewGeneration();
    void logGenerationStats(unsigned int generation) const;
    void saveGenerationData(unsigned int generation) const;
    
    double trainAgent(Indiv& agent, unsigned int generation);
    State getState(unsigned int x, unsigned int y, Direction dir) const;
    double calculateReward(unsigned int oldX, unsigned int newX, bool isNewState, bool goalReached) const;

    Parameters params_;
    Grid grid_;
    std::vector<Indiv> population_;
    unsigned long nextIndivId_;
    std::mt19937 randomEngine_;
    
    std::unique_ptr<DataLogger> logger_;
    
    std::vector<double> bestFitnessPerGen_;
    std::vector<double> avgFitnessPerGen_;
    std::map<unsigned long, std::pair<unsigned long, unsigned long>> parentMap_;  // creature_id -> (parent1, parent2)
};

} // namespace sim

#endif // SIMULATION_H
