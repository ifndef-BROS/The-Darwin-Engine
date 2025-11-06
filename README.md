# 🧬 TheDarwinEngine

A simulation engine designed for visualizing and managing evolving organisms and species within a dynamic simulation world.

---

## 📁 Project Structure

TheDarwinEngine/  
├── assets/  
│   └── Inter-Regular.ttf  
│  
├── build/  
│  
├── include/  
│   ├── Application.h  
│   ├── Config.h  
│   ├── GUIManager.h  
│   ├── SimulationWorld.h  
│   ├── Organism.h  
│   ├── SpeciesManager.h  
│   └── GraphRenderer.h  
│  
├── src/  
│   ├── Application.cpp  
│   ├── Config.cpp  
│   ├── GUIManager.cpp  
│   ├── SimulationWorld.cpp  
│   ├── Organism.cpp  
│   ├── SpeciesManager.cpp  
│   ├── GraphRenderer.cpp  
│   └── main.cpp  
│  
└── CMakeLists.txt  

---

## ⚙️ Setup & Build Instructions

1. **Open the project folder**  
   Save all the files in this exact structure in that folder.

2. **Open VS Code** in this folder.

3. **Set up the CMake kit:**  
   - Press `Ctrl + Shift + P`  
   - Select **“Select a Kit”**  
   - Choose **MinGW 15.2** or similar.

4. **Reconfigure the project:**  
   - Press `Ctrl + Shift + P` again  
   - Click on **“Delete Cache and Reconfigure…”**

5. **Build the project:**  
   - Press `Ctrl + Shift + P`  
   - Select **“Build”**

6. **Find the executable:**  
   - The application executable will be created in the **build/** folder  
   - The file will be named:  
     ```
     TheDarwinEngine
     ```

---

## 🧠 About

This project uses **CMake** for cross-platform builds and **MinGW** for Windows compilation.  
It includes modular headers and source files for handling simulation logic, species management, and GUI rendering.

---
