# Evolutionary RL Simulation - Mac Setup & Run Guide

## What This Project Does

This is an **interactive evolutionary simulation** where you can observe:
- **Creatures** learning and evolving in real-time
- **Gene inheritance** from parents to offspring
- **Mutations** occurring across generations
- **Learning progress** as creatures improve their Q-learning policies
- **Natural selection** where fit creatures survive and reproduce

## Prerequisites

### 1. Install Homebrew (if not already installed)
\`\`\`bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
\`\`\`

### 2. Install Required Tools
\`\`\`bash
# Install CMake
brew install cmake

# Install compiler (if not already installed)
brew install llvm

# Install Python 3 and visualization libraries
brew install python3
pip3 install matplotlib numpy
\`\`\`

## Step-by-Step Setup

### Step 1: Clone/Download the Project
\`\`\`bash
cd ~/Desktop  # or your preferred location
# If you have git:
git clone <your-repo-url> evolutionary-rl-sim
# Or download and extract the ZIP file
cd evolutionary-rl-sim
\`\`\`

### Step 2: Create Build Directory
\`\`\`bash
mkdir build
cd build
\`\`\`

### Step 3: Configure with CMake
\`\`\`bash
cmake ..
\`\`\`

You should see output like:
\`\`\`
-- The C compiler identification is AppleClang
-- The CXX compiler identification is AppleClang
-- Configuring done
-- Generating done
\`\`\`

### Step 4: Build the Project
\`\`\`bash
make -j4
\`\`\`

This compiles the C++ code. You should see:
\`\`\`
[100%] Built target evolutionary_rl
\`\`\`

### Step 5: Run the Simulation
\`\`\`bash
./evolutionary_rl
\`\`\`

The simulation will start and output progress:
\`\`\`
Generation 0: Population 50, Avg Fitness: 2.34, Max: 15.67
Generation 1: Population 50, Avg Fitness: 3.12, Max: 18.45
...
\`\`\`

This takes a few minutes depending on your parameters. The simulation will:
- Run 100 generations (configurable)
- Train 500 episodes per generation (configurable)
- Save detailed data to the `data/` folder

### Step 6: Visualize Results
Once the simulation completes, visualize the evolution:

\`\`\`bash
cd ..  # Go back to project root
python3 scripts/visualize_evolution.py
\`\`\`

This creates a `visualizations/` folder with 4 key plots:
1. **01_fitness_evolution.png** - How fitness improves over generations
2. **02_latest_generation_grid.png** - Where creatures are positioned
3. **03_gene_distribution.png** - How genes evolved
4. **04_mutation_heatmap.png** - Which creatures reproduced

## Understanding the Visualizations

### Fitness Evolution Plot
- **Blue line**: Average fitness across population
- **Orange line**: Best creature's fitness
- **Green line**: Worst creature's fitness
- **Shaded area**: Range of fitness values
- **What to look for**: Upward trend = evolution working!

### Grid Visualization
- **Green zone (right side)**: Goal area where creatures get rewards
- **Colored dots**: Creatures (color = learning success, size = fitness)
- **Arrows**: Direction each creature is facing
- **What to look for**: Creatures clustering toward goal zone

### Gene Distribution
- **4 histograms**: Show how each gene evolved
- **learning_rate**: How fast creatures learn (0-1)
- **discount_factor**: How much they value future rewards (0-1)
- **epsilon_decay**: How quickly they stop exploring
- **wrong_way_penalty**: Punishment for moving away from goal
- **What to look for**: Distributions shifting toward better values

### Mutation Heatmap
- **Red cells**: Parent pairs that produced many offspring
- **White cells**: Parent pairs that didn't reproduce
- **What to look for**: Diagonal pattern = best creatures reproducing

## Customizing the Simulation

Edit `include/parameters.h` to change:

\`\`\`cpp
struct Parameters {
    unsigned int populationSize = 50;        // Number of creatures
    unsigned int maxGenerations = 100;       // How many generations to run
    double mutationChance = 0.8;             // 80% chance of mutation
    double mutationSeverity = 0.2;           // How much genes change
    
    unsigned int trainingEpisodesPerGen = 500;  // Training per generation
    unsigned int gridWidth = 32;             // Grid size
    unsigned int gridHeight = 32;
    unsigned int stepsPerEpisode = 200;      // Max steps per episode
    
    double goalReward = 100.0;               // Reward for reaching goal
    double progressReward = 0.5;             // Reward for moving toward goal
    double wrongWayPenalty = -1.0;           // Penalty for wrong direction
};
\`\`\`

Then rebuild:
\`\`\`bash
cd build
cmake ..
make -j4
./evolutionary_rl
\`\`\`

## Troubleshooting

### "cmake: command not found"
\`\`\`bash
brew install cmake
\`\`\`

### "clang: error: unsupported option"
Update Xcode:
\`\`\`bash
xcode-select --install
\`\`\`

### "Python module not found"
\`\`\`bash
pip3 install matplotlib numpy
\`\`\`

### Simulation runs very slowly
- Reduce `populationSize` or `trainingEpisodesPerGen` in parameters.h
- Reduce `maxGenerations`
- Rebuild and run again

### No visualizations generated
Make sure `data/` folder exists and has JSON files:
\`\`\`bash
ls -la data/generations/
\`\`\`

If empty, the simulation didn't complete. Check for errors in the output.

## Project Structure

\`\`\`
evolutionary-rl-sim/
├── include/              # Header files
│   ├── common.h
│   ├── genome.h         # Gene definitions
│   ├── indiv.h          # Creature structure
│   ├── grid.h           # Environment
│   ├── neuralnet.h      # Neural network
│   ├── parameters.h     # Configuration
│   ├── data_logger.h    # Data logging
│   └── simulation.h     # Main simulation
├── src/                 # Implementation files
│   ├── grid.cpp
│   ├── neuralnet.cpp
│   ├── data_logger.cpp
│   ├── simulation.cpp
│   └── main.cpp
├── scripts/
│   └── visualize_evolution.py  # Visualization tool
├── data/                # Output data (created during run)
│   ├── generations/     # JSON files per generation
│   └── summary_stats.json
├── visualizations/      # Output plots (created after visualization)
├── CMakeLists.txt       # Build configuration
└── MAC_SETUP_GUIDE.md   # This file
\`\`\`

## Key Concepts Explained

### Genes (Genome)
Each creature has 4 genes that control how it learns:
- **Learning Rate**: How much it updates its knowledge (0.1-0.3)
- **Discount Factor**: How much it values future rewards (0.8-0.99)
- **Epsilon Decay**: How quickly it stops exploring (1000-5000)
- **Wrong Way Penalty**: How much it dislikes going wrong direction (-2 to 0)

### Mutation
When two creatures reproduce:
1. Offspring gets random mix of both parents' genes
2. Each gene has 20-30% chance to mutate (change randomly)
3. Better creatures' genes are more likely to survive

### Learning (Q-Learning)
Each creature learns during its lifetime:
1. Observes its position and direction (state)
2. Tries different actions (move, turn, stand)
3. Gets rewards/penalties based on progress
4. Updates its Q-table (memory of good actions)
5. Better Q-tables = better fitness

### Natural Selection
After each generation:
1. Creatures with high fitness reproduce more
2. Creatures with low fitness die
3. Top 10% always survive (elitism)
4. New generation inherits improved genes

## Next Steps

1. **Experiment**: Change parameters and see how evolution changes
2. **Analyze**: Look at the JSON files in `data/generations/` for detailed stats
3. **Extend**: Add new genes, new reward types, or new creature behaviors
4. **Visualize**: Create your own plots using the JSON data

## Questions?

Check the data files:
\`\`\`bash
cat data/generations/gen_0.json  # See first generation data
cat data/summary_stats.json      # See overall statistics
\`\`\`

Good luck with your project!
\`\`\`

```cmake file="" isHidden
