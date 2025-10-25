#ifndef NEURALNET_H
#define NEURALNET_H

#include "genome.h"
#include <vector>

namespace sim {

constexpr unsigned int MAX_NEURONS = 16;

class NeuralNet {
public:
    NeuralNet(const Genome& genome);
    void tick(const std::vector<double>& sensor_inputs);

    double output_move_forward = 0.0;
    double output_turn_right = 0.0;
    double output_turn_left = 0.0;

private:
    struct Connection {
        unsigned int source_neuron_idx;
        unsigned int sink_neuron_idx;
        double weight;
    };

    std::vector<Connection> connections_;
    double neurons_[MAX_NEURONS] = {0.0};
};

} // namespace sim

#endif // NEURALNET_H
