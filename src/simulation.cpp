#include "simulation.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <filesystem>

namespace sim {

Simulation::Simulation(const Parameters& params)
    : params_(params),
      grid_(params.gridWidth, params.gridHeight),
      population_(),
      nextIndivId_(0),
      logger_(std::make_unique<DataLogger>(params.outputDir)) {
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    randomEngine_.seed(seed);
    
    std::filesystem::create_directories(params_.outputDir);
    
    std::cout << "=== Evolutionary RL Simulation Initialized ===" << std::endl;
    std::cout << "Population: " << params_.populationSize << " | Generations: " << params_.maxGenerations << std::endl;
    std::cout << "Grid: " << params_.gridWidth << "x" << params_.gridHeight << " | Episodes/Gen: " << params_.trainingEpisodesPerGen << std::endl;
}

void Simulation::run() {
    initializePopulation();
    
    for (unsigned int gen = 0; gen < params_.maxGenerations; ++gen) {
        std::cout << "\n--- Generation " << gen << " ---" << std::endl;
        
        logger_->logGenerationStart(gen, params_.populationSize);
        
        // Train all individuals
        for (Indiv& indiv : population_) {
            std::cout << "." << std::flush;
            indiv.fitness = trainAgent(indiv, gen);
        }
        std::cout << std::endl;
        
        for (const auto& creature : population_) {
            // Log each creature's data (x=0 for now, y=middle, dir=EAST)
            logger_->logCreature(creature, 0, params_.gridHeight / 2, Direction::EAST, 0, 0);
        }
        
        // Selection and reproduction
        spawnNewGeneration();
        
        // Logging
        logGenerationStats(gen);
        saveGenerationData(gen);
        
        logger_->logGenerationEnd(gen);
    }
    
    logger_->saveSummaryStats();
    
    std::cout << "\n=== Evolution Complete ===" << std::endl;
    std::cout << "Data saved to: " << params_.outputDir << std::endl;
}

void Simulation::initializePopulation() {
    population_.clear();
    
    std::uniform_real_distribution<double> rate_dist(0.05, 0.3);
    std::uniform_real_distribution<double> factor_dist(0.8, 0.99);
    std::uniform_real_distribution<double> decay_dist(500.0, 5000.0);
    std::uniform_real_distribution<double> penalty_dist(-2.0, -0.1);
    
    for (unsigned int i = 0; i < params_.populationSize; ++i) {
        Genome g;
        g.learningRate = rate_dist(randomEngine_);
        g.discountFactor = factor_dist(randomEngine_);
        g.epsilonDecayRate = decay_dist(randomEngine_);
        g.wrongWayPenalty = penalty_dist(randomEngine_);
        population_.emplace_back(g, nextIndivId_++);
    }
    
    std::cout << "Population initialized with " << params_.populationSize << " individuals." << std::endl;
}

void Simulation::spawnNewGeneration() {
    if (population_.empty()) return;
    
    std::sort(population_.begin(), population_.end(), [](const Indiv& a, const Indiv& b) {
        return a.fitness < b.fitness;
    });

    std::vector<Indiv> newPopulation;
    
    int elite_count = (int)(params_.populationSize * params_.elitePreservationRate);
    for (int i = 0; i < elite_count; ++i) {
        newPopulation.push_back(population_[population_.size() - 1 - i]);
    }

    int breeding_pool_size = (int)(params_.populationSize * 0.5);
    if (breeding_pool_size == 0) breeding_pool_size = 1;
    
    std::uniform_int_distribution<int> parent_dist(population_.size() - breeding_pool_size, population_.size() - 1);
    std::uniform_real_distribution<double> mut_chance(0.0, 1.0);
    std::normal_distribution<double> mut_dist(0.0, params_.mutationSeverity);

    while (newPopulation.size() < params_.populationSize) {
        Indiv& parent1 = population_[parent_dist(randomEngine_)];
        Indiv& parent2 = population_[parent_dist(randomEngine_)];
        
        Genome childGenome = parent1.genome;
        
        std::map<std::string, double> mutations;
        
        if (mut_chance(randomEngine_) < childGenome.learningRateMutationRate) {
            double old_val = childGenome.learningRate;
            childGenome.learningRate *= (1.0 + mut_dist(randomEngine_));
            mutations["learning_rate"] = childGenome.learningRate - old_val;
        }
        if (mut_chance(randomEngine_) < childGenome.discountFactorMutationRate) {
            double old_val = childGenome.discountFactor;
            childGenome.discountFactor *= (1.0 + mut_dist(randomEngine_));
            mutations["discount_factor"] = childGenome.discountFactor - old_val;
        }
        if (mut_chance(randomEngine_) < childGenome.epsilonDecayMutationRate) {
            double old_val = childGenome.epsilonDecayRate;
            childGenome.epsilonDecayRate *= (1.0 + mut_dist(randomEngine_));
            mutations["epsilon_decay"] = childGenome.epsilonDecayRate - old_val;
        }
        if (mut_chance(randomEngine_) < childGenome.penaltyMutationRate) {
            double old_val = childGenome.wrongWayPenalty;
            childGenome.wrongWayPenalty *= (1.0 + mut_dist(randomEngine_));
            mutations["wrong_way_penalty"] = childGenome.wrongWayPenalty - old_val;
        }
        
        // Clamp values to valid ranges
        childGenome.learningRate = std::max(0.001, std::min(childGenome.learningRate, 1.0));
        childGenome.discountFactor = std::max(0.7, std::min(childGenome.discountFactor, 0.999));
        childGenome.epsilonDecayRate = std::max(100.0, std::min(childGenome.epsilonDecayRate, 10000.0));
        childGenome.wrongWayPenalty = std::max(-5.0, std::min(childGenome.wrongWayPenalty, 0.0));
        
        unsigned long offspring_id = nextIndivId_++;
        newPopulation.emplace_back(childGenome, offspring_id);
        
        logger_->logMutation(parent1.id, parent2.id, offspring_id, mutations, 
                            parent1.fitness, parent2.fitness);
    }
    
    population_ = newPopulation;
}

void Simulation::logGenerationStats(unsigned int generation) const {
    if (population_.empty()) return;
    
    const Indiv& best = *std::max_element(population_.begin(), population_.end(), 
        [](const Indiv& a, const Indiv& b) { return a.fitness < b.fitness; });
    
    double avgFitness = 0.0;
    for (const auto& indiv : population_) {
        avgFitness += indiv.fitness;
    }
    avgFitness /= population_.size();
    
    std::cout << "Gen " << std::setw(3) << generation 
              << " | Best: " << std::fixed << std::setprecision(4) << best.fitness
              << " | Avg: " << avgFitness
              << " | Successful Episodes: " << best.successfulEpisodes << "/" << params_.trainingEpisodesPerGen
              << std::endl;
}

void Simulation::saveGenerationData(unsigned int generation) const {
    if (!params_.saveQTables) return;
    
    const Indiv& best = *std::max_element(population_.begin(), population_.end(), 
        [](const Indiv& a, const Indiv& b) { return a.fitness < b.fitness; });
    
    std::string filename = params_.outputDir + "/q_table_gen_" + std::to_string(generation) + ".csv";
    std::ofstream file(filename);
    
    if (!file.is_open()) return;
    
    file << "x_bucket,y_bucket,direction,q_move,q_turn_r,q_turn_l,q_still\n";
    
    for (const auto& [state, qvalues] : best.Q_table) {
        file << state.x_bucket << "," << state.y_bucket << "," << (int)state.dir;
        for (double q : qvalues) {
            file << "," << q;
        }
        file << "\n";
    }
    
    file.close();
}

State Simulation::getState(unsigned int x, unsigned int y, Direction dir) const {
    State s;
    s.x_bucket = (int)(x / (double)params_.gridWidth * params_.stateBucketsX);
    s.y_bucket = (int)(y / (double)params_.gridHeight * params_.stateBucketsY);
    s.dir = dir;
    return s;
}

double Simulation::calculateReward(unsigned int oldX, unsigned int newX, bool isNewState, bool goalReached) const {
    double reward = params_.moveReward;
    
    if (newX > oldX) {
        reward += params_.progressReward;
    } else if (newX < oldX) {
        reward += params_.wrongWayPenalty;
    }
    
    if (isNewState) {
        reward += params_.explorationBonus;
    }
    
    if (goalReached) {
        reward = params_.goalReward;
    }
    
    return reward;
}

double Simulation::trainAgent(Indiv& agent, unsigned int generation) {
    double learningRate = agent.genome.learningRate;
    double discountFactor = agent.genome.discountFactor;
    double wrongWayReward = agent.genome.wrongWayPenalty;
    
    double initialEpsilon = 1.0, finalEpsilon = 0.01;
    double epsilonDecay = (agent.genome.epsilonDecayRate > 0) ? 
        ((initialEpsilon - finalEpsilon) / agent.genome.epsilonDecayRate) : 0;
    double epsilon = initialEpsilon;
    
    agent.successfulEpisodes = 0;
    double totalReward = 0.0;
    agent.visited_states.clear();
    
    for (unsigned int episode = 0; episode < params_.trainingEpisodesPerGen; ++episode) {
        unsigned int x = 0, y = params_.gridHeight / 2;
        Direction dir = Direction::EAST;
        double episodeReward = 0.0;
        
        for (unsigned int step = 0; step < params_.stepsPerEpisode; ++step) {
            State currentState = getState(x, y, dir);
            bool isNewState = agent.visited_states.find(currentState) == agent.visited_states.end();
            agent.visited_states.insert(currentState);
            
            if (agent.Q_table.find(currentState) == agent.Q_table.end()) {
                agent.Q_table[currentState] = {0.0, 0.0, 0.0, 0.0};
            }
            
            Action chosenAction;
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            if (dist(randomEngine_) < epsilon) {
                std::uniform_int_distribution<int> actionDist(0, 3);
                chosenAction = (Action)actionDist(randomEngine_);
            } else {
                auto q_values = agent.Q_table[currentState];
                chosenAction = (Action)std::distance(q_values.begin(), std::max_element(q_values.begin(), q_values.end()));
            }
            
            unsigned int oldX = x;
            State oldState = currentState;
            
            if (chosenAction == Action::MOVE_FWD) {
                if (dir == Direction::NORTH && y > 0) y--;
                else if (dir == Direction::EAST && x < params_.gridWidth - 1) x++;
                else if (dir == Direction::SOUTH && y < params_.gridHeight - 1) y++;
                else if (dir == Direction::WEST && x > 0) x--;
            } else if (chosenAction == Action::TURN_RIGHT) {
                dir = (Direction)(((int)dir + 1) % 4);
            } else if (chosenAction == Action::TURN_LEFT) {
                dir = (Direction)(((int)dir + 3) % 4);
            }
            
            State newState = getState(x, y, dir);
            bool goalReached = (x >= params_.goalX);
            
            double reward = calculateReward(oldX, x, isNewState, goalReached);
            episodeReward += reward;
            
            if (agent.Q_table.find(newState) == agent.Q_table.end()) {
                agent.Q_table[newState] = {0.0, 0.0, 0.0, 0.0};
            }
            
            double old_q = agent.Q_table[oldState][(int)chosenAction];
            double max_future_q = *std::max_element(agent.Q_table[newState].begin(), agent.Q_table[newState].end());
            agent.Q_table[oldState][(int)chosenAction] = old_q + learningRate * (reward + discountFactor * max_future_q - old_q);
            
            if (goalReached) {
                agent.successfulEpisodes++;
                break;
            }
        }
        
        totalReward += episodeReward;
        if (epsilon > finalEpsilon) epsilon -= epsilonDecay;
    }
    
    agent.avgRewardPerEpisode = totalReward / params_.trainingEpisodesPerGen;
    return (double)agent.successfulEpisodes / params_.trainingEpisodesPerGen;
}

} // namespace sim
