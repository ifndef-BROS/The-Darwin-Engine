# Detailed Guide - Understanding the Evolutionary RL Simulation

## Project Overview

This is a **dual-learning system** where creatures evolve genetically AND learn individually:

\`\`\`
┌─────────────────────────────────────────────────────────────┐
│         EVOLUTIONARY RL SIMULATION ARCHITECTURE             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  GENERATION LOOP (100 generations)                         │
│  ├─ TRAINING PHASE (each creature trains)                  │
│  │  ├─ Episode 1-500                                       │
│  │  │  ├─ Observe state (position, direction)              │
│  │  │  ├─ Choose action (explore or exploit)               │
│  │  │  ├─ Get reward (progress toward goal)                │
│  │  │  └─ Update Q-table (learn)                           │
│  │  └─ Result: Fitness = success rate                      │
│  │                                                          │
│  ├─ SELECTION PHASE (natural selection)                    │
│  │  ├─ Sort by fitness                                     │
│  │  ├─ Keep top 10% (elitism)                              │
│  │  └─ Breed rest from top 50%                             │
│  │                                                          │
│  └─ MUTATION PHASE (genetic variation)                     │
│     ├─ Offspring inherit parent genes                      │
│     ├─ Each gene has 20-30% mutation chance                │
│     └─ Mutations create diversity                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
\`\`\`

## Key Concepts

### 1. Genes (What Creatures Inherit)

Each creature has 4 genes controlling how it learns:

| Gene | Range | Meaning | Effect |
|------|-------|---------|--------|
| **learning_rate** | 0.001-1.0 | How fast to learn | Higher = faster learning but less stable |
| **discount_factor** | 0.7-0.999 | Value of future rewards | Higher = plan further ahead |
| **epsilon_decay** | 100-10000 | Exploration schedule | Higher = explore longer |
| **wrong_way_penalty** | -5 to 0 | Punishment for wrong direction | More negative = stronger penalty |

**Example:**
- Creature A: learning_rate=0.2, discount_factor=0.95 (balanced learner)
- Creature B: learning_rate=0.05, discount_factor=0.99 (slow but far-sighted)
- Creature C: learning_rate=0.3, discount_factor=0.8 (fast but short-sighted)

### 2. Mutations (How Genes Change)

When two creatures reproduce:

\`\`\`
Parent 1 (Fitness: 0.8)          Parent 2 (Fitness: 0.6)
├─ learning_rate: 0.20           ├─ learning_rate: 0.15
├─ discount_factor: 0.95         ├─ discount_factor: 0.92
├─ epsilon_decay: 2000           ├─ epsilon_decay: 1500
└─ wrong_way_penalty: -1.0       └─ wrong_way_penalty: -0.8

                    ↓ REPRODUCTION ↓

Offspring (inherits from Parent 1)
├─ learning_rate: 0.20 → 0.22 (mutated +10%)
├─ discount_factor: 0.95 (no mutation)
├─ epsilon_decay: 2000 → 1800 (mutated -10%)
└─ wrong_way_penalty: -1.0 (no mutation)
\`\`\`

**Mutation Rates:**
- 30% chance learning_rate mutates
- 20% chance discount_factor mutates
- 25% chance epsilon_decay mutates
- 20% chance wrong_way_penalty mutates

### 3. Learning (Q-Learning)

Each creature learns during its lifetime using Q-learning:

\`\`\`
State = (position_bucket, direction)
Action = (move_forward, turn_right, turn_left, stand_still)

Q-table[state][action] = expected reward for taking action in state

Update rule:
Q_new = Q_old + learning_rate * (reward + discount_factor * max_future_Q - Q_old)
\`\`\`

**Example Learning:**
\`\`\`
Creature at position (0, 16), facing EAST
├─ Tries MOVE_FORWARD
├─ Reaches position (1, 16)
├─ Gets reward: +0.5 (progress toward goal)
├─ Updates Q-table: Q[(0,16)][MOVE_FORWARD] += 0.2 * (0.5 + 0.95 * max_future_Q - old_Q)
└─ Next time in this state, more likely to move forward
\`\`\`

### 4. Natural Selection (Survival of the Fittest)

After training, creatures are ranked by fitness:

\`\`\`
Generation N:
Creature 1: Fitness 0.95 ✓ Survives (top 10%)
Creature 2: Fitness 0.92 ✓ Survives (top 10%)
Creature 3: Fitness 0.88 ✓ Survives (top 10%)
Creature 4: Fitness 0.85 ✗ Dies
Creature 5: Fitness 0.82 ✗ Dies
...
Creature 50: Fitness 0.01 ✗ Dies

Generation N+1:
├─ Top 3 creatures copied (elitism)
├─ Remaining 47 bred from top 25 creatures
└─ New genes + mutations = new population
\`\`\`

## Understanding the Visualizations

### 1. Fitness Evolution (01_fitness_evolution.png)

\`\`\`
Fitness
   ↑
   │     ╱╱╱ Best (max)
   │    ╱╱╱╱╱
   │   ╱╱╱╱╱╱╱ Average
   │  ╱╱╱╱╱╱╱╱╱
   │ ╱╱╱╱╱╱╱╱╱╱╱ Worst (min)
   └─────────────────→ Generation
\`\`\`

**What to look for:**
- Upward trend = evolution working
- Plateau = population converged
- Sudden jumps = good mutations found

### 2. Grid Visualization (02_latest_generation_grid.png)

\`\`\`
32 ┌─────────────────────────────────────┐
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║ GOAL
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║ ZONE
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
   │ ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● ║
 0 └─────────────────────────────────────┘
   0                                    32
\`\`\`

- **Dot color**: Green = learned well, Red = didn't learn
- **Dot size**: Larger = higher fitness
- **Arrow**: Direction creature is facing
- **Green zone**: Goal area (right side)

### 3. Gene Distribution (03_gene_distribution.png)

Shows how genes evolved:

\`\`\`
Learning Rate Distribution (Gen 99)
Frequency
    ↑
    │     ╱╲
    │    ╱  ╲
    │   ╱    ╲
    │  ╱      ╲
    │ ╱        ╲
    └──────────────→ Learning Rate Value
    0.05  0.15  0.25
\`\`\`

**What to look for:**
- Narrow peak = population converged on good value
- Wide spread = still exploring
- Shift from Gen 0 to Gen 99 = evolution happened

### 4. Gene Evolution Timeline (05_gene_evolution_timeline.png)

Shows how each gene changed over generations:

\`\`\`
Learning Rate Over Time
Value
  0.3 ├─────────────────────────────
      │    ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
  0.2 ├───╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
      │  ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
  0.1 ├─╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
      │╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
  0.0 └─────────────────────────────
      0    25    50    75    100
      Generation
\`\`\`

- **Line**: Mean value
- **Shaded area**: ±1 standard deviation
- **Upward trend**: Gene improving
- **Narrowing band**: Population converging

### 5. Learning Progress (06_learning_progress.png)

Shows how creatures learned:

\`\`\`
Avg Reward Per Episode
    ↑
    │     ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    │    ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    │   ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    │  ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    │ ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    └─────────────────────────────→ Generation
\`\`\`

- **Upward trend**: Creatures learning better strategies
- **Steep slope**: Fast learning
- **Plateau**: Converged to optimal strategy

### 6. Inheritance Tree (07_inheritance_tree.png)

Shows which creatures reproduced most:

\`\`\`
Parent Reproductive Success
Parent 1 ████████████████ 16 offspring
Parent 2 ████████████ 12 offspring
Parent 3 ████████ 8 offspring
Parent 4 ████ 4 offspring
Parent 5 ██ 2 offspring
\`\`\`

- **Longer bars**: Better genes (more offspring)
- **Shorter bars**: Worse genes (fewer offspring)
- **Diagonal pattern**: Natural selection working

## Customizing the Simulation

Edit `include/parameters.h`:

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

**Experiment Ideas:**
1. Increase `populationSize` to 100 - more diversity
2. Increase `trainingEpisodesPerGen` to 1000 - better learning
3. Decrease `mutationSeverity` to 0.1 - more stable evolution
4. Increase `goalReward` to 200 - stronger goal incentive

## Project Structure

\`\`\`
evolutionary-rl-sim/
├── include/                    # Header files
│   ├── common.h               # Enums and constants
│   ├── genome.h               # Gene definitions
│   ├── indiv.h                # Creature structure
│   ├── grid.h                 # Environment
│   ├── neuralnet.h            # Neural network (optional)
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
├── scripts/                    # Python tools
│   ├── visualize_evolution.py      # Basic plots
│   ├── visualize_detailed.py       # Detailed analysis
│   ├── interactive_explorer.py     # Data explorer
│   └── run_all_visualizations.py   # Master script
│
├── data/                       # Output (created during run)
│   ├── generations/            # JSON per generation
│   └── summary_stats.json      # Overall statistics
│
├── visualizations/             # Output plots (created after visualization)
│   ├── 01_fitness_evolution.png
│   ├── 02_latest_generation_grid.png
│   ├── 03_gene_distribution.png
│   ├── 04_mutation_heatmap.png
│   ├── 05_gene_evolution_timeline.png
│   ├── 06_learning_progress.png
│   └── 07_inheritance_tree.png
│
├── CMakeLists.txt              # Build configuration
├── QUICKSTART.md               # Quick start guide
├── DETAILED_GUIDE.md           # This file
└── MAC_SETUP_GUIDE.md          # Mac-specific setup
\`\`\`

## Common Questions

**Q: Why do creatures die in early generations?**
A: They haven't learned yet! The RL system needs time to discover good strategies. By generation 10-20, you should see improvement.

**Q: Why is fitness between 0 and 1?**
A: Fitness = (successful episodes) / (total episodes). So 0.5 means 50% success rate.

**Q: Can I run this on Linux/Windows?**
A: Yes! The code is platform-independent. Just install CMake and a C++17 compiler.

**Q: How do I speed up the simulation?**
A: Reduce `populationSize`, `trainingEpisodesPerGen`, or `maxGenerations` in parameters.h

**Q: Can I add new genes?**
A: Yes! Add to `Genome` struct in genome.h, then update mutation logic in simulation.cpp

## Next Steps

1. **Run the simulation**: Follow QUICKSTART.md
2. **Explore the data**: Use `interactive_explorer.py`
3. **Modify parameters**: Experiment with different settings
4. **Extend the code**: Add new genes, reward types, or behaviors
5. **Analyze results**: Create your own visualizations

Good luck with your project!
