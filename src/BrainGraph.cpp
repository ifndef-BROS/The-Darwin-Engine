#include "rl_brain/BrainGraph.hpp"
#include <boost/graph/graph_utility.hpp>

BrainGraph::BrainGraph() {}

void BrainGraph::add_neuron(const std::string& type) {
    NeuronDescriptor v = boost::add_vertex(graph);
    graph[v].type = type;
    graph[v].activation = 0.0;
}

void BrainGraph::add_synapse(int from, int to, double weight) {
    auto edge_pair = boost::add_edge(from, to, graph);
    SynapseDescriptor e = edge_pair.first;
    graph[e].weight = weight;
}

void BrainGraph::print_graph() {
    std::cout << "--- Brain Graph Structure ---" << std::endl;
    boost::print_graph(graph, boost::get(&Neuron::type, graph));
    std::cout << "---------------------------" << std::endl;
}
