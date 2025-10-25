#ifndef INDIV_H
#define INDIV_H

#include "genome.h"
#include <vector>
#include <map>
#include <set>

namespace sim {

enum class Direction { NORTH, EAST, SOUTH, WEST };
enum class Action { MOVE_FWD, TURN_RIGHT, TURN_LEFT, STAND_STILL };

struct State {
    int x_bucket;
    int y_bucket;
    Direction dir;
    
    bool operator<(const State& other) const {
        if (x_bucket != other.x_bucket) return x_bucket < other.x_bucket;
        if (y_bucket != other.y_bucket) return y_bucket < other.y_bucket;
        return dir < other.dir;
    }
    
    bool operator==(const State& other) const {
        return x_bucket == other.x_bucket && y_bucket == other.y_bucket && dir == other.dir;
    }
};

struct Indiv {
    // --- The Innate ---
    Genome genome;

    // --- The Learned ---
    std::map<State, std::vector<double>> Q_table;
    std::set<State> visited_states;  // Track exploration for bonus rewards
    
    // --- Other Attributes ---
    unsigned long id = 0;
    double fitness = 0.0;
    double avgRewardPerEpisode = 0.0;  // Track learning progress
    int successfulEpisodes = 0;        // Count successful episodes

    Indiv(const Genome& g, unsigned long unique_id) : genome(g), id(unique_id) {}
    Indiv() = default;
};

} // namespace sim

#endif // INDIV_H
