#!/usr/bin/env python3
"""
Interactive explorer for examining specific generations and creatures
"""

import json
import glob
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

class InteractiveExplorer:
    def __init__(self, data_dir="data"):
        self.data_dir = data_dir
        self.generations = []
        self.load_data()
    
    def load_data(self):
        gen_files = sorted(glob.glob(f"{self.data_dir}/generations/gen_*.json"))
        for gen_file in gen_files:
            with open(gen_file, 'r') as f:
                self.generations.append(json.load(f))
        print(f"Loaded {len(self.generations)} generations")
    
    def show_generation_summary(self, gen_idx):
        """Print detailed summary of a generation"""
        if gen_idx >= len(self.generations):
            print(f"Generation {gen_idx} not found")
            return
        
        gen = self.generations[gen_idx]
        print(f"\n{'='*60}")
        print(f"GENERATION {gen['generation']} SUMMARY")
        print('='*60)
        print(f"Population Size: {gen['population_size']}")
        print(f"Average Fitness: {gen['avg_fitness']:.4f}")
        print(f"Max Fitness: {gen['max_fitness']:.4f}")
        print(f"Min Fitness: {gen['min_fitness']:.4f}")
        print(f"Avg Successful Episodes: {gen['avg_successful_episodes']:.2f}")
        print(f"Total Mutations: {len(gen['mutations'])}")
        print('='*60)
    
    def show_top_creatures(self, gen_idx, top_n=5):
        """Show top N creatures in a generation"""
        if gen_idx >= len(self.generations):
            return
        
        gen = self.generations[gen_idx]
        creatures = sorted(gen['creatures'], key=lambda c: c['fitness'], reverse=True)
        
        print(f"\nTop {top_n} Creatures in Generation {gen['generation']}:")
        print("-" * 80)
        print(f"{'Rank':<5} {'ID':<8} {'Fitness':<10} {'Episodes':<10} {'Learning Rate':<15} {'Discount':<10}")
        print("-" * 80)
        
        for i, creature in enumerate(creatures[:top_n], 1):
            print(f"{i:<5} {creature['id']:<8} {creature['fitness']:<10.4f} "
                  f"{creature['successful_episodes']:<10} {creature['genome']['learning_rate']:<15.4f} "
                  f"{creature['genome']['discount_factor']:<10.4f}")
        print()
    
    def compare_generations(self, gen_idx1, gen_idx2):
        """Compare two generations"""
        if gen_idx1 >= len(self.generations) or gen_idx2 >= len(self.generations):
            print("Invalid generation indices")
            return
        
        gen1 = self.generations[gen_idx1]
        gen2 = self.generations[gen_idx2]
        
        print(f"\n{'='*60}")
        print(f"COMPARING GENERATION {gen1['generation']} vs {gen2['generation']}")
        print('='*60)
        print(f"{'Metric':<30} {'Gen {gen1['generation']}':<15} {'Gen {gen2['generation']}':<15} {'Change':<15}")
        print('-'*60)
        
        metrics = [
            ('Avg Fitness', gen1['avg_fitness'], gen2['avg_fitness']),
            ('Max Fitness', gen1['max_fitness'], gen2['max_fitness']),
            ('Avg Episodes', gen1['avg_successful_episodes'], gen2['avg_successful_episodes']),
        ]
        
        for name, val1, val2 in metrics:
            change = ((val2 - val1) / val1 * 100) if val1 != 0 else 0
            print(f"{name:<30} {val1:<15.4f} {val2:<15.4f} {change:+.1f}%")
        print()
    
    def interactive_menu(self):
        """Interactive menu for exploring data"""
        while True:
            print("\n" + "="*60)
            print("INTERACTIVE EVOLUTION EXPLORER")
            print("="*60)
            print("1. Show generation summary")
            print("2. Show top creatures in a generation")
            print("3. Compare two generations")
            print("4. List all generations")
            print("5. Exit")
            print("="*60)
            
            choice = input("Enter choice (1-5): ").strip()
            
            if choice == '1':
                gen_idx = int(input(f"Enter generation index (0-{len(self.generations)-1}): "))
                self.show_generation_summary(gen_idx)
            
            elif choice == '2':
                gen_idx = int(input(f"Enter generation index (0-{len(self.generations)-1}): "))
                top_n = int(input("How many top creatures to show? "))
                self.show_top_creatures(gen_idx, top_n)
            
            elif choice == '3':
                gen1 = int(input(f"Enter first generation (0-{len(self.generations)-1}): "))
                gen2 = int(input(f"Enter second generation (0-{len(self.generations)-1}): "))
                self.compare_generations(gen1, gen2)
            
            elif choice == '4':
                print("\nAvailable Generations:")
                for i, gen in enumerate(self.generations):
                    print(f"  Gen {i}: Avg Fitness={gen['avg_fitness']:.4f}, "
                          f"Max={gen['max_fitness']:.4f}, Population={gen['population_size']}")
            
            elif choice == '5':
                print("Exiting...")
                break
            
            else:
                print("Invalid choice")

if __name__ == "__main__":
    explorer = InteractiveExplorer("data")
    explorer.interactive_menu()
