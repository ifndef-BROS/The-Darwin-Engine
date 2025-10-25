#!/usr/bin/env python3
"""
Detailed visualization showing:
- How genes mutate across generations
- How creatures learn (Q-values)
- Inheritance patterns
"""

import json
import glob
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
from pathlib import Path

class DetailedVisualizer:
    def __init__(self, data_dir="data"):
        self.data_dir = data_dir
        self.generations = []
        self.load_data()
    
    def load_data(self):
        gen_files = sorted(glob.glob(f"{self.data_dir}/generations/gen_*.json"))
        for gen_file in gen_files:
            with open(gen_file, 'r') as f:
                self.generations.append(json.load(f))
    
    def visualize_gene_mutation_timeline(self):
        """Show how each gene evolves over time"""
        if not self.generations:
            return
        
        genes = ['learning_rate', 'discount_factor', 'epsilon_decay', 'wrong_way_penalty']
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle('Gene Evolution Timeline - How Genes Change Across Generations', 
                    fontsize=16, fontweight='bold')
        
        for idx, (ax, gene) in enumerate(zip(axes.flat, genes)):
            generations_list = []
            means = []
            stds = []
            
            for gen_data in self.generations:
                gen_num = gen_data['generation']
                values = [c['genome'][gene] for c in gen_data['creatures']]
                
                generations_list.append(gen_num)
                means.append(np.mean(values))
                stds.append(np.std(values))
            
            # Plot mean with error band
            ax.plot(generations_list, means, 'o-', linewidth=2.5, markersize=6, label='Mean')
            ax.fill_between(generations_list, 
                           np.array(means) - np.array(stds),
                           np.array(means) + np.array(stds),
                           alpha=0.3, label='±1 Std Dev')
            
            ax.set_xlabel('Generation', fontsize=11)
            ax.set_ylabel(gene.replace('_', ' ').title(), fontsize=11)
            ax.set_title(f'{gene.replace("_", " ").title()} Evolution', fontsize=12, fontweight='bold')
            ax.legend(fontsize=10)
            ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def visualize_inheritance_tree(self, generation_idx):
        """Show parent-offspring relationships"""
        if generation_idx >= len(self.generations):
            return
        
        gen_data = self.generations[generation_idx]
        mutations = gen_data['mutations']
        
        if not mutations:
            print(f"No mutations in generation {generation_idx}")
            return
        
        fig, ax = plt.subplots(figsize=(14, 8))
        
        # Count offspring per parent
        parent_offspring = {}
        for m in mutations:
            p1 = m['parent1_id']
            p2 = m['parent2_id']
            
            if p1 not in parent_offspring:
                parent_offspring[p1] = 0
            if p2 not in parent_offspring:
                parent_offspring[p2] = 0
            
            parent_offspring[p1] += 1
            parent_offspring[p2] += 1
        
        # Sort by offspring count
        sorted_parents = sorted(parent_offspring.items(), key=lambda x: x[1], reverse=True)
        parent_ids = [p[0] for p in sorted_parents]
        offspring_counts = [p[1] for p in sorted_parents]
        
        colors = plt.cm.RdYlGn(np.linspace(0.3, 0.9, len(parent_ids)))
        bars = ax.barh(range(len(parent_ids)), offspring_counts, color=colors, edgecolor='black', linewidth=1.5)
        
        ax.set_yticks(range(len(parent_ids)))
        ax.set_yticklabels([f'Parent {pid}' for pid in parent_ids], fontsize=10)
        ax.set_xlabel('Number of Offspring', fontsize=12, fontweight='bold')
        ax.set_title(f'Generation {generation_idx} - Reproductive Success\n'
                    f'(How many offspring each parent produced)',
                    fontsize=14, fontweight='bold')
        ax.grid(axis='x', alpha=0.3)
        
        # Add value labels on bars
        for i, (bar, count) in enumerate(zip(bars, offspring_counts)):
            ax.text(count + 0.1, i, str(count), va='center', fontsize=10, fontweight='bold')
        
        plt.tight_layout()
        return fig
    
    def visualize_learning_progress(self):
        """Show how creatures learn over generations"""
        if not self.generations:
            return
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        generations_list = []
        best_rewards = []
        avg_rewards = []
        
        for gen_data in self.generations:
            gen_num = gen_data['generation']
            rewards = [c['avg_reward'] for c in gen_data['creatures']]
            
            generations_list.append(gen_num)
            best_rewards.append(max(rewards))
            avg_rewards.append(np.mean(rewards))
        
        # Plot 1: Reward trends
        ax1.plot(generations_list, best_rewards, 's-', label='Best Reward', linewidth=2.5, markersize=6)
        ax1.plot(generations_list, avg_rewards, 'o-', label='Avg Reward', linewidth=2.5, markersize=6)
        ax1.fill_between(generations_list, avg_rewards, best_rewards, alpha=0.2)
        ax1.set_xlabel('Generation', fontsize=12)
        ax1.set_ylabel('Average Reward per Episode', fontsize=12)
        ax1.set_title('Learning Progress - Reward Trends', fontsize=13, fontweight='bold')
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Success rate
        success_rates = []
        for gen_data in self.generations:
            episodes = [c['successful_episodes'] for c in gen_data['creatures']]
            success_rates.append(np.mean(episodes))
        
        ax2.plot(generations_list, success_rates, 'o-', color='green', linewidth=2.5, markersize=6)
        ax2.fill_between(generations_list, success_rates, alpha=0.3, color='green')
        ax2.set_xlabel('Generation', fontsize=12)
        ax2.set_ylabel('Avg Successful Episodes', fontsize=12)
        ax2.set_title('Learning Progress - Success Rate', fontsize=13, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def create_full_report(self):
        """Generate complete analysis report"""
        print("\n" + "="*70)
        print("DETAILED EVOLUTIONARY ANALYSIS REPORT")
        print("="*70)
        
        if not self.generations:
            print("No data found!")
            return
        
        output_dir = "visualizations"
        Path(output_dir).mkdir(exist_ok=True)
        
        # 1. Gene evolution timeline
        print("\n[1/3] Creating gene evolution timeline...")
        fig = self.visualize_gene_mutation_timeline()
        fig.savefig(f"{output_dir}/05_gene_evolution_timeline.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        # 2. Learning progress
        print("[2/3] Creating learning progress analysis...")
        fig = self.visualize_learning_progress()
        fig.savefig(f"{output_dir}/06_learning_progress.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        # 3. Inheritance tree (latest generation)
        print("[3/3] Creating inheritance analysis...")
        fig = self.visualize_inheritance_tree(len(self.generations) - 1)
        fig.savefig(f"{output_dir}/07_inheritance_tree.png", dpi=150, bbox_inches='tight')
        plt.close(fig)
        
        # Print statistics
        print("\n" + "-"*70)
        print("SIMULATION STATISTICS")
        print("-"*70)
        print(f"Total Generations: {len(self.generations)}")
        print(f"Initial Avg Fitness: {self.generations[0]['avg_fitness']:.4f}")
        print(f"Final Avg Fitness: {self.generations[-1]['avg_fitness']:.4f}")
        print(f"Best Fitness Achieved: {max(g['max_fitness'] for g in self.generations):.4f}")
        print(f"Improvement: {((self.generations[-1]['avg_fitness'] - self.generations[0]['avg_fitness']) / self.generations[0]['avg_fitness'] * 100):.1f}%")
        print("="*70 + "\n")

if __name__ == "__main__":
    visualizer = DetailedVisualizer("data")
    visualizer.create_full_report()
