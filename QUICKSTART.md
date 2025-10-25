# Quick Start Guide - Evolutionary RL Simulation on Mac

## TL;DR - Get Running in 5 Minutes

### Prerequisites (one-time setup)
\`\`\`bash
# Install Homebrew (if needed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install cmake python3
pip3 install matplotlib numpy pandas
\`\`\`

### Build & Run
\`\`\`bash
# Navigate to project (adjust path if different)
cd ~/Desktop/biosim4

# Build
mkdir -p build && cd build
cmake ..
make -j4
cd ..

# Run simulation (takes 2-5 minutes)
./build/evolutionary_rl

# Visualize results
python3 scripts/run_all_visualizations.py

# View results
open visualizations/01_fitness_evolution.png
\`\`\`

## What You'll See

**Console Output:**
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
...
\`\`\`

**Generated Visualizations:**
- `01_fitness_evolution.png` - Fitness improving over time
- `02_latest_generation_grid.png` - Where creatures ended up
- `03_gene_distribution.png` - Gene value distributions
- `04_mutation_heatmap.png` - Which creatures reproduced
- `05_gene_evolution_timeline.png` - Gene changes over generations
- `06_learning_progress.png` - Learning curves
- `07_inheritance_tree.png` - Reproductive success

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `cmake: command not found` | `brew install cmake` |
| `clang: error` | `xcode-select --install` |
| `ModuleNotFoundError: matplotlib` | `pip3 install matplotlib numpy pandas` |
| `No such file or directory: build/evolutionary_rl` | Make sure you ran `cmake ..` and `make -j4` in the build folder |
| Simulation too slow | Reduce `populationSize` or `trainingEpisodesPerGen` in `include/parameters.h` |
| No visualizations generated | Run `./build/evolutionary_rl` first to generate data in `data/` folder |

## Next Steps

1. **Explore the data**: `python3 scripts/interactive_explorer.py`
2. **Modify parameters**: Edit `include/parameters.h` and rebuild
3. **Understand the code**: Read `DETAILED_GUIDE.md`
