# Evolutionary Reinforcement Learning Simulation

A sophisticated simulation combining evolutionary algorithms with reinforcement learning (Q-learning) to evolve agents that learn to navigate a grid environment. This is a complete data structures project showcasing evolution, learning, mutation, and natural selection.

## Features

- **Dual Learning System**: Combines genetic evolution (across generations) with individual RL learning (within lifetime)
- **Improved Reward Shaping**: Progress-based rewards, exploration bonuses, and goal rewards
- **Adaptive Epsilon**: Epsilon-greedy exploration that adapts over training episodes
- **Gene-Level Mutation**: Fine-grained control over which genes mutate
- **Data Persistence**: Saves detailed generation data and Q-tables for analysis
- **7 Comprehensive Visualizations**: See evolution, learning, and mutation in action
- **Interactive Tools**: Explore data and compare generations

## Quick Start (5 Minutes)

### Prerequisites
\`\`\`bash
# Install Homebrew (if needed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install cmake python3
pip3 install matplotlib numpy
\`\`\`

### Build & Run
\`\`\`bash
cd evolutionary-rl-sim
mkdir build && cd build
cmake ..
make -j4
./evolutionary_rl

# After simulation completes:
cd ..
python3 scripts/run_all_visualizations.py
open visualizations/01_fitness_evolution.png
\`\`\`

## What You'll See

### Console Output
\`\`\`
=== Evolutionary RL Simulation Initialized ===
Population: 50 | Generations: 100
Grid: 32x32 | Episodes/Gen: 500

--- Generation 0 ---
..................................................
Gen   0 | Best: 0.1234 | Avg: 0.0567 | Successful Episodes: 45/500

--- Generation 1 ---
..................................................
Gen   1 | Best: 0.2456 | Avg: 0.1123 | Successful Episodes: 78/500
\`\`\`

### Generated Visualizations
1. **01_fitness_evolution.png** - Fitness improving over generations
2. **02_latest_generation_grid.png** - Where creatures learned to go
3. **03_gene_distribution.png** - Gene value distributions
4. **04_mutation_heatmap.png** - Which creatures reproduced
5. **05_gene_evolution_timeline.png** - How genes evolved
6. **06_learning_progress.png** - Learning curves and success rates
7. **07_inheritance_tree.png** - Reproductive success

## Understanding the Simulation

### The 4 Genes (What Creatures Inherit)

Each creature has genes controlling how it learns:

| Gene | Range | Meaning |
|------|-------|---------|
| **learning_rate** | 0.001-1.0 | How fast to learn |
| **discount_factor** | 0.7-0.999 | Value of future rewards |
| **epsilon_decay** | 100-10000 | Exploration schedule |
| **wrong_way_penalty** | -5 to 0 | Punishment for wrong direction |

### How Evolution Works

\`\`\`
Generation N:
├─ Training: Each creature learns via Q-learning (500 episodes)
├─ Evaluation: Fitness = success rate
├─ Selection: Top 10% survive, breed from top 50%
└─ Mutation: Offspring inherit genes + random mutations

Generation N+1:
└─ Repeat with new population
\`\`\`

### How Learning Works

Each creature learns during its lifetime:
\`\`\`
State = (position_bucket, direction)
Action = (move_forward, turn_right, turn_left, stand_still)

Q-table[state][action] = expected reward

Update: Q_new = Q_old + α * (reward + γ * max_future_Q - Q_old)
\`\`\`

## Configuration

Edit `include/parameters.h` to customize:

\`\`\`cpp
struct Parameters {
    // Population control
    unsigned int populationSize = 50;        // More = slower but better diversity
    unsigned int maxGenerations = 100;       // More = longer evolution
    
    // Mutation control
    double mutationChance = 0.8;             // 80% of offspring mutate
    double mutationSeverity = 0.2;           // ±20% gene change
    double elitePreservationRate = 0.1;      // Top 10% always survive
    
    // Training control
    unsigned int trainingEpisodesPerGen = 500;  // More = better learning
    unsigned int stepsPerEpisode = 200;         // Max steps per episode
    
    // Reward tuning
    double goalReward = 100.0;               // Reward for reaching goal
    double progressReward = 0.5;             // Reward for moving right
    double wrongWayPenalty = -1.0;           // Penalty for moving left
    double explorationBonus = 0.1;           // Bonus for new states
};
\`\`\`

## Interactive Tools

### Explore Specific Generations
\`\`\`bash
python3 scripts/interactive_explorer.py
\`\`\`

Menu options:
- Show generation summary
- Show top creatures
- Compare two generations
- List all generations

### Run All Visualizations
\`\`\`bash
python3 scripts/run_all_visualizations.py
\`\`\`

## Project Structure

\`\`\`
evolutionary-rl-sim/
├── include/                    # Header files
│   ├── common.h               # Enums and constants
│   ├── genome.h               # Gene definitions
│   ├── indiv.h                # Creature structure
│   ├── grid.h                 # Environment
│   ├── parameters.h           # Configuration
│   ├── data_logger.h          # Data logging
│   └── simulation.h           # Main simulation
│
├── src/                        # Implementation
│   ├── main.cpp               # Entry point
│   ├── simulation.cpp         # Simulation logic
│   ├── grid.cpp               # Grid implementation
│   ├── neuralnet.cpp          # Neural network
│   └── data_logger.cpp        # Data logging
│
├── scripts/                    # Python visualization tools
│   ├── visualize_evolution.py      # Basic plots
│   ├── visualize_detailed.py       # Detailed analysis
│   ├── interactive_explorer.py     # Data explorer
│   └── run_all_visualizations.py   # Master script
│
├── data/                       # Output (created during run)
│   ├── generations/            # JSON per generation
│   └── summary_stats.json      # Overall statistics
│
├── visualizations/             # Output plots
│   ├── 01_fitness_evolution.png
│   ├── 02_latest_generation_grid.png
│   ├── 03_gene_distribution.png
│   ├── 04_mutation_heatmap.png
│   ├── 05_gene_evolution_timeline.png
│   ├── 06_learning_progress.png
│   └── 07_inheritance_tree.png
│
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── QUICKSTART.md               # Quick start guide
└── DETAILED_GUIDE.md           # Deep dive into concepts
\`\`\`

## Key Improvements

1. **Better Reward Shaping**: Agents receive rewards for progress, exploration, and goal achievement
2. **Adaptive Learning**: Epsilon decay and state-based exploration bonuses
3. **Gene-Level Control**: Each gene has its own mutation rate
4. **Elite Preservation**: Top performers always survive to next generation
5. **Comprehensive Data Logging**: Track all aspects of evolution and learning
6. **7 Visualizations**: See evolution, learning, and mutation in action
7. **Interactive Tools**: Explore and analyze the data
8. **Complete Documentation**: QUICKSTART.md and DETAILED_GUIDE.md

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `cmake: command not found` | `brew install cmake` |
| `clang: error` | `xcode-select --install` |
| `ModuleNotFoundError: matplotlib` | `pip3 install matplotlib numpy` |
| Simulation too slow | Reduce `populationSize` or `trainingEpisodesPerGen` |
| No visualizations | Check `data/generations/` has JSON files |

## References

- Original inspiration: [biosim4](https://github.com/davidrmiller/biosim4)
- Q-Learning: Watkins & Dayan (1992)
- Genetic Algorithms: Holland (1975)

## Next Steps

1. **Run the simulation**: Follow the Quick Start section
2. **Explore the data**: Use `interactive_explorer.py`
3. **Modify parameters**: Experiment with different settings
4. **Extend the code**: Add new genes, reward types, or behaviors
5. **Analyze results**: Create your own visualizations

For detailed explanations, see `DETAILED_GUIDE.md` and `QUICKSTART.md`.
