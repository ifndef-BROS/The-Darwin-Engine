#ifndef GENOME_H
#define GENOME_H

namespace sim {

struct Genome {
    double learningRate = 0.15;
    double discountFactor = 0.95;
    double epsilonDecayRate = 2000.0;
    double wrongWayPenalty = -1.0;
    
    double learningRateMutationRate = 0.3;
    double discountFactorMutationRate = 0.2;
    double epsilonDecayMutationRate = 0.25;
    double penaltyMutationRate = 0.2;
};

} // namespace sim

#endif // GENOME_H
