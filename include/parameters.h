#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>

namespace sim {

struct Parameters {
    // --- Evolutionary Parameters ---
    unsigned int populationSize = 50;
    unsigned int maxGenerations = 100;
    double mutationChance = 0.8;
    double mutationSeverity = 0.2;
    double elitePreservationRate = 0.1;  // Top 10% always survive

    // --- Reinforcement Learning Parameters ---
    unsigned int trainingEpisodesPerGen = 500;
    unsigned int gridWidth = 32;
    unsigned int gridHeight = 32;
    unsigned int stepsPerEpisode = 200;
    
    double moveReward = -0.05;           // Small cost per step (encourages efficiency)
    double goalReward = 100.0;           // Large reward for reaching goal
    double progressReward = 0.5;         // Reward for moving toward goal
    double wrongWayPenalty = -1.0;       // Penalty for moving away from goal
    double explorationBonus = 0.1;       // Bonus for visiting new states
    
    unsigned int goalX = gridWidth - 1;
    
    unsigned int stateBucketsX = 8;
    unsigned int stateBucketsY = 8;
    
    std::string outputDir = "data";
    bool saveQTables = true;
    bool logVerbose = false;
};

} // namespace sim

#endif // PARAMETERS_H
