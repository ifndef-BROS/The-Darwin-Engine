#!/usr/bin/env python3
"""
Visualize the learned Q-table policy from the evolutionary RL simulation.
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
from pathlib import Path

def plot_policy_map(q_table_path='data/q_table_gen_99.csv'):
    """
    Reads the trained Q-Table from the C++ simulation and visualizes
    the agent's learned policy as a vector field.
    """
    try:
        df = pd.read_csv(q_table_path)
        print(f"Successfully loaded {q_table_path} with {len(df)} states.")
    except FileNotFoundError:
        print(f"Error: Could not find the file at '{q_table_path}'.")
        print("Please make sure you have run the C++ simulation first.")
        return

    directions = {0: 'North', 1: 'East', 2: 'South', 3: 'West'}
    action_columns = ['q_move', 'q_turn_r', 'q_turn_l', 'q_still']

    df['best_action_idx'] = df[action_columns].idxmax(axis=1).apply(lambda x: action_columns.index(x))
    
    fig, axes = plt.subplots(2, 2, figsize=(15, 15))
    fig.suptitle('Agent\'s Learned Policy (Best Action from Each State)', fontsize=16)
    
    axes = axes.flatten()

    for dir_idx, ax in enumerate(axes):
        ax.set_title(f'Facing {directions[dir_idx]}')
        ax.set_xlim(-0.5, 7.5)
        ax.set_ylim(-0.5, 7.5)
        ax.set_aspect('equal', adjustable='box')
        ax.set_xticks(np.arange(0, 8, 1))
        ax.set_yticks(np.arange(0, 8, 1))
        ax.grid(True, linestyle='--', alpha=0.6)

        subset = df[df['direction'] == dir_idx]
        
        if subset.empty:
            print(f"No data for direction {directions[dir_idx]}.")
            continue

        for _, row in subset.iterrows():
            x, y = row['x_bucket'], row['y_bucket']
            action = row['best_action_idx']
            
            if action == 0:   # Move Forward
                if dir_idx == 0:
                    dx, dy = 0, 0.4
                elif dir_idx == 1:
                    dx, dy = 0.4, 0
                elif dir_idx == 2:
                    dx, dy = 0, -0.4
                else:
                    dx, dy = -0.4, 0
                ax.arrow(x, y, dx, dy, head_width=0.2, head_length=0.2, fc='green', ec='green')
            elif action == 1:
                ax.text(x, y, '↷', fontsize=20, ha='center', va='center', color='blue')
            elif action == 2:
                ax.text(x, y, '↶', fontsize=20, ha='center', va='center', color='red')
            else:
                ax.scatter(x, y, s=100, c='gray', marker='o')

    plt.tight_layout(rect=[0, 0, 1, 0.96])
    output_filename = 'policy_map.png'
    plt.savefig(output_filename)
    print(f"\nPolicy map saved as '{output_filename}'")

if __name__ == '__main__':
    if len(sys.argv) > 1:
        plot_policy_map(sys.argv[1])
    else:
        plot_policy_map()
