#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include "indiv.h"
#include "parameters.h"

namespace sim {

struct CreatureSnapshot {
    unsigned long id;
    int x, y;
    Direction direction;
    double fitness;
    double avg_reward;
    int successful_episodes;
    int age;
    unsigned long parent1_id;
    unsigned long parent2_id;
    std::map<std::string, double> genome_values;
};

struct MutationRecord {
    int generation;
    unsigned long parent1_id;
    unsigned long parent2_id;
    unsigned long offspring_id;
    std::map<std::string, double> mutations;  // gene_name -> mutation_amount
    double parent1_fitness;
    double parent2_fitness;
};

struct GenerationSnapshot {
    int generation;
    int population_size;
    double avg_fitness;
    double max_fitness;
    double min_fitness;
    double avg_successful_episodes;
    std::vector<CreatureSnapshot> creatures;
    std::vector<MutationRecord> mutations;
};

class DataLogger {
public:
    DataLogger(const std::string& output_dir = "data");
    ~DataLogger();
    
    void logGenerationStart(int generation, int population_size);
    void logCreature(const Indiv& creature, int x, int y, Direction dir, 
                     unsigned long parent1_id = 0, unsigned long parent2_id = 0);
    void logMutation(unsigned long parent1_id, unsigned long parent2_id,
                     unsigned long offspring_id, const std::map<std::string, double>& mutations,
                     double parent1_fitness, double parent2_fitness);
    void logGenerationEnd(int generation);
    
    void saveGenerationData(int generation);
    void saveSummaryStats();
    
private:
    std::string output_dir_;
    std::vector<GenerationSnapshot> generations_;
    GenerationSnapshot current_generation_;
    
    void ensureOutputDir();
};

} // namespace sim
