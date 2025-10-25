#!/usr/bin/env python3
"""
Master visualization script - runs all visualizations and creates a comprehensive report
"""

import subprocess
import sys
from pathlib import Path

def run_visualizations():
    """Run all visualization scripts"""
    print("\n" + "="*70)
    print("EVOLUTIONARY SIMULATION - COMPLETE VISUALIZATION SUITE")
    print("="*70)
    
    scripts = [
        ("scripts/visualize_evolution.py", "Basic Evolution Visualizations"),
        ("scripts/visualize_detailed.py", "Detailed Analysis & Gene Evolution"),
    ]
    
    for script, description in scripts:
        print(f"\n{'='*70}")
        print(f"Running: {description}")
        print(f"Script: {script}")
        print('='*70)
        
        try:
            result = subprocess.run([sys.executable, script], check=True)
            if result.returncode != 0:
                print(f"Warning: {script} exited with code {result.returncode}")
        except subprocess.CalledProcessError as e:
            print(f"Error running {script}: {e}")
        except FileNotFoundError:
            print(f"Script not found: {script}")
    
    print("\n" + "="*70)
    print("VISUALIZATION COMPLETE!")
    print("="*70)
    print("\nGenerated files in 'visualizations/' directory:")
    print("  01_fitness_evolution.png - Fitness trends across generations")
    print("  02_latest_generation_grid.png - Creature positions in final generation")
    print("  03_gene_distribution.png - Gene value distributions")
    print("  04_mutation_heatmap.png - Reproduction patterns")
    print("  05_gene_evolution_timeline.png - How genes evolved over time")
    print("  06_learning_progress.png - Learning curves and success rates")
    print("  07_inheritance_tree.png - Reproductive success of each creature")
    print("\n" + "="*70 + "\n")

if __name__ == "__main__":
    run_visualizations()
