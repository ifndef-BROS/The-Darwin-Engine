#!/usr/bin/env python3
"""
Interactive visualization of evolutionary simulation.
Shows creatures moving, learning, and evolving across generations.
"""

import json
import os
import glob
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.animation as animation
from matplotlib.patches import FancyArrowPatch
import numpy as np
from pathlib import Path

class EvolutionVisualizer:
    def __init__(self, data_dir="data"):
        self.data_dir = data_dir
        self.generations = []
        self.load_data()
        
    def load_data(self):
        """Load all generation data from JSON files"""
        gen_files = sorted(glob.glob(f"{self.data_dir}/generations/gen_*.json"))
        for gen_file in gen_files:
            with open(gen_file, 'r') as f:
                self.generations.append(json.load(f))
        print(f"Loaded {len(self.generations)} generations")
    
    def visualize_grid_with_creatures(self, generation_idx, grid_width=32, grid_height=32):
        """Visualize creatures on the grid for a specific generation"""
        if generation_idx >= len(self.generations):
            print(f"Generation {generation_idx} not found")
            return
        
        gen_data = self.generations[generation_idx]
        creatures = gen_data['creatures']
        
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Draw grid
        ax.set_xlim(0, grid_width)
        ax.set_ylim(0, grid_height)
        ax.set_aspect('equal')
        ax.invert_yaxis()
        
        # Draw grid lines
        for i in range(grid_width + 1):
            ax.axvline(i, color='lightgray', linewidth=0.5, alpha=0.3)
        for i in range(grid_height + 1):
            ax.axhline(i, color='lightgray', linewidth=0.5, alpha=0.3)
        
        # Draw goal zone (right side)
        goal_rect = patches.Rectangle((grid_width - 4, 0), 4, grid_height, 
                                      linewidth=2, edgecolor='green', 
                                      facecolor='lightgreen', alpha=0.3)
        ax.add_patch(goal_rect)
        ax.text(grid_width - 2, -1.5, 'GOAL ZONE', ha='center', fontsize=10, 
                fontweight='bold', color='green')
        
        # Plot creatures
        colors = plt.cm.viridis(np.linspace(0, 1, len(creatures)))
        for idx, creature in enumerate(creatures):
            x, y = creature['x'], creature['y']
            fitness = creature['fitness']
            
            # Size based on fitness
            size = 50 + (fitness / 10)
            
            # Color based on successful episodes
            color_intensity = min(creature['successful_episodes'] / 10, 1.0)
            color = plt.cm.RdYlGn(color_intensity)
            
            ax.scatter(x, y, s=size, c=[color], alpha=0.7, edgecolors='black', linewidth=1)
            
            # Draw direction arrow
            direction_map = {0: (0, -0.3), 1: (0.3, 0), 2: (0, 0.3), 3: (-0.3, 0)}
            dx, dy = direction_map[creature.get('direction', 0)]
            ax.arrow(x, y, dx, dy, head_width=0.15, head_length=0.1, fc='black', ec='black')
        
        ax.set_xlabel('X Position', fontsize=12)
        ax.set_ylabel('Y Position', fontsize=12)
        ax.set_title(f'Generation {gen_data["generation"]} - Population Distribution\n'
                    f'Avg Fitness: {gen_data["avg_fitness"]:.2f} | '
                    f'Max: {gen_data["max_fitness"]:.2f}', fontsize=14, fontweight='bold')
        
        plt.tight_layout()
        return fig
    
    def visualize_gene_inheritance(self, generation_idx):
        """Visualize gene inheritance tree for a generation"""
        if generation_idx >= len(self.generations):
            return
        
        gen_data = self.generations[generation_idx]
        creatures = gen_data['creatures']
        
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle(f'Generation {gen_data["generation"]} - Gene Distribution', 
                    fontsize=16, fontweight='bold')
        
        genes = ['learning_rate', 'discount_factor', 'epsilon_decay', 'wrong_way_penalty']
        
        for idx, (ax, gene) in enumerate(zip(axes.flat, genes)):
            values = [c['genome'][gene] for c in creatures]
            
            ax.hist(values, bins=15, color='steelblue', alpha=0.7, edgecolor='black')
            ax.set_xlabel(gene.replace('_', ' ').title(), fontsize=11)
            ax.set_ylabel('Frequency', fontsize=11)
            ax.set_title(f'{gene.replace("_", " ").title()}\n'
                        f'Mean: {np.mean(values):.4f} | Std: {np.std(values):.4f}',
                        fontsize=10)
            ax.grid(axis='y', alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def visualize_fitness_evolution(self):
        """Show fitness trends across all generations"""
        if not self.generations:
            return
        
        generations = [g['generation'] for g in self.generations]
        avg_fitness = [g['avg_fitness'] for g in self.generations]
        max_fitness = [g['max_fitness'] for g in self.generations]
        min_fitness = [g['min_fitness'] for g in self.generations]
        
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
        
        # Fitness trends
        ax1.plot(generations, avg_fitness, 'o-', label='Average Fitness', linewidth=2, markersize=6)
        ax1.plot(generations, max_fitness, 's--', label='Max Fitness', linewidth=2, markersize=5, alpha=0.7)
        ax1.plot(generations, min_fitness, '^--', label='Min Fitness', linewidth=2, markersize=5, alpha=0.7)
        ax1.fill_between(generations, min_fitness, max_fitness, alpha=0.2)
        ax1.set_xlabel('Generation', fontsize=12)
        ax1.set_ylabel('Fitness', fontsize=12)
        ax1.set_title('Fitness Evolution Across Generations', fontsize=14, fontweight='bold')
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)
        
        # Learning progress
        avg_episodes = [g['avg_successful_episodes'] for g in self.generations]
        ax2.plot(generations, avg_episodes, 'o-', color='green', linewidth=2, markersize=6)
        ax2.fill_between(generations, avg_episodes, alpha=0.3, color='green')
        ax2.set_xlabel('Generation', fontsize=12)
        ax2.set_ylabel('Avg Successful Episodes', fontsize=12)
        ax2.set_title('Learning Progress - Successful Episodes Per Generation', fontsize=14, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def visualize_mutation_heatmap(self, generation_idx):
        """Show mutation patterns in a generation"""
        if generation_idx >= len(self.generations):
            return
        
        gen_data = self.generations[generation_idx]
        mutations = gen_data['mutations']
        
        if not mutations:
            print(f"No mutations recorded in generation {generation_idx}")
            return
        
        fig, ax = plt.subplots(figsize=(12, 8))
        
        # Create mutation matrix
        parent_ids = set()
        for m in mutations:
            parent_ids.add(m['parent1_id'])
            parent_ids.add(m['parent2_id'])
        
        parent_list = sorted(list(parent_ids))
        parent_to_idx = {pid: i for i, pid in enumerate(parent_list)}
        
        mutation_matrix = np.zeros((len(parent_list), len(parent_list)))
        
        for m in mutations:
            p1_idx = parent_to_idx[m['parent1_id']]
            p2_idx = parent_to_idx[m['parent2_id']]
            mutation_matrix[p1_idx, p2_idx] += 1
        
        im = ax.imshow(mutation_matrix, cmap='YlOrRd', aspect='auto')
        ax.set_xlabel('Parent 2 ID', fontsize=12)
        ax.set_ylabel('Parent 1 ID', fontsize=12)
        ax.set_title(f'Generation {gen_data["generation"]} - Mutation Heatmap\n'
                    f'(How many offspring each parent pair produced)',
                    fontsize=14, fontweight='bold')
        
        plt.colorbar(im, ax=ax, label='Number of Offspring')
        plt.tight_layout()
        return fig
    
    def create_interactive_dashboard(self):
        """Create an interactive dashboard showing all visualizations"""
        print("\n" + "="*60)
        print("EVOLUTIONARY SIMULATION VISUALIZATION DASHBOARD")
        print("="*60)
        
        if not self.generations:
            print("No generation data found!")
            return
        
        print(f"\nTotal Generations: {len(self.generations)}")
        print(f"Final Average Fitness: {self.generations[-1]['avg_fitness']:.2f}")
        print(f"Best Fitness Achieved: {max(g['max_fitness'] for g in self.generations):.2f}")
        
        # Create output directory
        output_dir = "visualizations"
        Path(output_dir).mkdir(exist_ok=True)
        
        # 1. Fitness evolution
        print("\n[1/4] Creating fitness evolution plot...")
        fig = self.visualize_fitness_evolution()
        fig.savefig(f"{output_dir}/01_fitness_evolution.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        # 2. Latest generation grid
        print("[2/4] Creating latest generation grid visualization...")
        fig = self.visualize_grid_with_creatures(len(self.generations) - 1)
        fig.savefig(f"{output_dir}/02_latest_generation_grid.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        # 3. Gene distribution
        print("[3/4] Creating gene distribution plots...")
        fig = self.visualize_gene_inheritance(len(self.generations) - 1)
        fig.savefig(f"{output_dir}/03_gene_distribution.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        # 4. Mutation heatmap
        print("[4/4] Creating mutation heatmap...")
        fig = self.visualize_mutation_heatmap(len(self.generations) - 1)
        fig.savefig(f"{output_dir}/04_mutation_heatmap.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        print(f"\nAll visualizations saved to '{output_dir}/' directory!")
        print("="*60 + "\n")

if __name__ == "__main__":
    visualizer = EvolutionVisualizer("data")
    visualizer.create_interactive_dashboard()
