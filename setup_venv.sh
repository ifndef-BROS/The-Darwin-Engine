#!/bin/bash

# Create Python virtual environment
python3 -m venv venv

# Activate virtual environment
source venv/bin/activate

# Install required packages
pip install matplotlib numpy pandas

echo "✓ Virtual environment created and packages installed!"
echo "✓ To use it in the future, run: source venv/bin/activate"
