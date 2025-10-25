#include "data_logger.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <cmath>

namespace sim {

DataLogger::DataLogger(const std::string& output_dir) 
    : output_dir_(output_dir) {
    ensureOutputDir();
}

DataLogger::~DataLogger() = default;

void DataLogger::ensureOutputDir() {
    std::filesystem::create_directories(output_dir_);
    std::filesystem::create_directories(output_dir_ + "/generations");
    std::filesystem::create_directories(output_dir_ + "/mutations");
}

void DataLogger::logGenerationStart(int generation, int population_size) {
    current_generation_.generation = generation;
    current_generation_.population_size = population_size;
    current_generation_.creatures.clear();
    current_generation_.mutations.clear();
}

void DataLogger::logCreature(const Indiv& creature, int x, int y, Direction dir,
                             unsigned long parent1_id, unsigned long parent2_id) {
    CreatureSnapshot snapshot;
    snapshot.id = creature.id;
    snapshot.x = x;
    snapshot.y = y;
    snapshot.direction = dir;
    snapshot.fitness = creature.fitness;
    snapshot.avg_reward = creature.avgRewardPerEpisode;
    snapshot.successful_episodes = creature.successfulEpisodes;
    snapshot.parent1_id = parent1_id;
    snapshot.parent2_id = parent2_id;
    
    snapshot.genome_values["learning_rate"] = creature.genome.learningRate;
    snapshot.genome_values["discount_factor"] = creature.genome.discountFactor;
    snapshot.genome_values["epsilon_decay"] = creature.genome.epsilonDecayRate;
    snapshot.genome_values["wrong_way_penalty"] = creature.genome.wrongWayPenalty;
    
    current_generation_.creatures.push_back(snapshot);
}

void DataLogger::logMutation(unsigned long parent1_id, unsigned long parent2_id,
                             unsigned long offspring_id, const std::map<std::string, double>& mutations,
                             double parent1_fitness, double parent2_fitness) {
    MutationRecord record;
    record.generation = current_generation_.generation;
    record.parent1_id = parent1_id;
    record.parent2_id = parent2_id;
    record.offspring_id = offspring_id;
    record.mutations = mutations;
    record.parent1_fitness = parent1_fitness;
    record.parent2_fitness = parent2_fitness;
    
    current_generation_.mutations.push_back(record);
}

void DataLogger::logGenerationEnd(int generation) {
    if (current_generation_.creatures.empty()) return;
    
    double total_fitness = 0.0;
    double max_fitness = -1e9;
    double min_fitness = 1e9;
    double total_episodes = 0.0;
    
    for (const auto& creature : current_generation_.creatures) {
        total_fitness += creature.fitness;
        max_fitness = std::max(max_fitness, creature.fitness);
        min_fitness = std::min(min_fitness, creature.fitness);
        total_episodes += creature.successful_episodes;
    }
    
    current_generation_.avg_fitness = total_fitness / current_generation_.creatures.size();
    current_generation_.max_fitness = max_fitness;
    current_generation_.min_fitness = min_fitness;
    current_generation_.avg_successful_episodes = total_episodes / current_generation_.creatures.size();
    
    generations_.push_back(current_generation_);
    saveGenerationData(generation);
}

void DataLogger::saveGenerationData(int generation) {
    std::string filename = output_dir_ + "/generations/gen_" + 
                          std::to_string(generation) + ".json";
    std::ofstream file(filename);
    
    file << "{\n";
    file << "  \"generation\": " << current_generation_.generation << ",\n";
    file << "  \"population_size\": " << current_generation_.population_size << ",\n";
    file << "  \"avg_fitness\": " << std::fixed << std::setprecision(4) 
         << current_generation_.avg_fitness << ",\n";
    file << "  \"max_fitness\": " << current_generation_.max_fitness << ",\n";
    file << "  \"min_fitness\": " << current_generation_.min_fitness << ",\n";
    file << "  \"avg_successful_episodes\": " << current_generation_.avg_successful_episodes << ",\n";
    file << "  \"creatures\": [\n";
    
    for (size_t i = 0; i < current_generation_.creatures.size(); ++i) {
        const auto& c = current_generation_.creatures[i];
        file << "    {\n";
        file << "      \"id\": " << c.id << ",\n";
        file << "      \"x\": " << c.x << ",\n";
        file << "      \"y\": " << c.y << ",\n";
        file << "      \"fitness\": " << std::fixed << std::setprecision(4) << c.fitness << ",\n";
        file << "      \"avg_reward\": " << c.avg_reward << ",\n";
        file << "      \"successful_episodes\": " << c.successful_episodes << ",\n";
        file << "      \"parent1_id\": " << c.parent1_id << ",\n";
        file << "      \"parent2_id\": " << c.parent2_id << ",\n";
        file << "      \"genome\": {\n";
        file << "        \"learning_rate\": " << c.genome_values.at("learning_rate") << ",\n";
        file << "        \"discount_factor\": " << c.genome_values.at("discount_factor") << ",\n";
        file << "        \"epsilon_decay\": " << c.genome_values.at("epsilon_decay") << ",\n";
        file << "        \"wrong_way_penalty\": " << c.genome_values.at("wrong_way_penalty") << "\n";
        file << "      }\n";
        file << "    }";
        if (i < current_generation_.creatures.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ],\n";
    file << "  \"mutations\": [\n";
    
    for (size_t i = 0; i < current_generation_.mutations.size(); ++i) {
        const auto& m = current_generation_.mutations[i];
        file << "    {\n";
        file << "      \"parent1_id\": " << m.parent1_id << ",\n";
        file << "      \"parent2_id\": " << m.parent2_id << ",\n";
        file << "      \"offspring_id\": " << m.offspring_id << ",\n";
        file << "      \"parent1_fitness\": " << std::fixed << std::setprecision(4) 
             << m.parent1_fitness << ",\n";
        file << "      \"parent2_fitness\": " << m.parent2_fitness << "\n";
        file << "    }";
        if (i < current_generation_.mutations.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    file.close();
}

void DataLogger::saveSummaryStats() {
    std::string filename = output_dir_ + "/summary_stats.json";
    std::ofstream file(filename);
    
    file << "{\n";
    file << "  \"total_generations\": " << generations_.size() << ",\n";
    file << "  \"generations\": [\n";
    
    for (size_t i = 0; i < generations_.size(); ++i) {
        const auto& gen = generations_[i];
        file << "    {\n";
        file << "      \"generation\": " << gen.generation << ",\n";
        file << "      \"avg_fitness\": " << std::fixed << std::setprecision(4) 
             << gen.avg_fitness << ",\n";
        file << "      \"max_fitness\": " << gen.max_fitness << ",\n";
        file << "      \"min_fitness\": " << gen.min_fitness << ",\n";
        file << "      \"avg_successful_episodes\": " << gen.avg_successful_episodes << "\n";
        file << "    }";
        if (i < generations_.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    file.close();
}

} // namespace sim
