#include "rl_brain/BrainGraph.hpp"

int main() {
    BrainGraph my_brain;

    // Create a simple neural network structure
    my_brain.add_neuron("input");  // Neuron 0
    my_brain.add_neuron("input");  // Neuron 1
    my_brain.add_neuron("hidden"); // Neuron 2
    my_brain.add_neuron("output"); // Neuron 3

    my_brain.add_synapse(0, 2, 0.5);
    my_brain.add_synapse(1, 2, 0.8);
    my_brain.add_synapse(2, 3, 0.9);

    my_brain.print_graph();

    return 0;
}
