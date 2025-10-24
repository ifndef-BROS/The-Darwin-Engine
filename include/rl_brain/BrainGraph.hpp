#ifndef BRAIN_GRAPH_HPP
#define BRAIN_GRAPH_HPP

#include <boost/graph/adjacency_list.hpp>
#include <iostream>

// Define the properties of our neurons (vertices) and synapses (edges)
struct Neuron {
    std::string type; // e.g., "input", "hidden", "output"
    double activation;
};

struct Synapse {
    double weight;
};

// Define the graph type using boost::adjacency_list
using BrainGraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, Neuron, Synapse>;
using NeuronDescriptor = boost::graph_traits<BrainGraphType>::vertex_descriptor;
using SynapseDescriptor = boost::graph_traits<BrainGraphType>::edge_descriptor;

class BrainGraph {
public:
    BrainGraph();
    void add_neuron(const std::string& type);
    void add_synapse(int from, int to, double weight);
    void print_graph();

private:
    BrainGraphType graph;
};

#endif // BRAIN_GRAPH_HPP
