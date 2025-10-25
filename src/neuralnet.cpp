#include "neuralnet.h"
#include "common.h"
#include <cmath>

namespace sim {

NeuralNet::NeuralNet(const Genome& genome) {
    connections_.clear();
    // Placeholder for neural network construction
    // In this version, we focus on RL-based learning
}

void NeuralNet::tick(const std::vector<double>& sensor_inputs) {
    // Placeholder for neural network tick
    // RL agents use Q-tables instead of neural networks
}

} // namespace sim
