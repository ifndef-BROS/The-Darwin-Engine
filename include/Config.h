#pragma once

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Holds all static and dynamic configuration settings.
 *
 * This struct centralizes all parameters, from window layout and theme
 * colors to the core simulation variables that can be edited by the GUI.
 */
struct Config
{
    // =========================================
    // 1. EVOLUTION PARAMETERS (Editable via GUI)
    // =========================================
    // These are the "base stats" for Species 0, the initial organism.

    /**
     * @brief Chance per second for a *new* Species 0 organism
     * to spontaneously appear. This trait is NOT inherited.
     */
    float initialSpawnRate{0.5f}; // 50% chance/sec

    /**
     * @brief Base chance per second for an organism to replicate.
     * This trait IS inherited and mutates.
     */
    float initialReplicationRate{0.05f}; // 5% chance/sec

    /**
     * @brief Base chance per second for an organism to die.
     * This trait IS inherited and mutates.
     */
    float initialDeathRate{0.02f}; // 2% chance/sec

    /**
     * @brief Base chance for an offspring to be a new species.
     * This trait IS inherited and mutates.
     */
    float initialMutationRate{0.10f}; // 10% chance

    // =========================================
    // 2. SIMULATION CONSTANTS
    // =========================================

    /**
     * @brief The 'C' value. The resource factor approaches 0
     * as the population approaches this number.
     */
    int carryingCapacity{2000};

    /**
     * @brief How many organisms to create on simulation start/reset.
     * Set to 0 because we will rely on initialSpawnRate.
     */
    int initialPopulation{0};

    /**
     * @brief How frequently to record data for the graph (in seconds).
     */
    float graphUpdateInterval{0.25f}; // 4 times per second

    /**
     * @brief The % amount a gene can change during mutation
     * (e.g., 0.01 = +/- 1% change).
     */
    float mutationAmount{0.02f}; // 2% variance

    /**
     * @brief Visual size of the organisms.
     */
    float organismRadius{4.0f};

    /**
     * @brief Movement speed of the organisms.
     */
    float organismSpeed{60.0f}; // pixels per second

    // =========================================
    // 3. WINDOW & LAYOUT
    // =========================================
    unsigned int windowWidth{1280};
    unsigned int windowHeight{720};
    std::string windowTitle{"TheDarwinEngine - Evolution Simulator"};
    unsigned int framerateLimit{60};

    /**
     * @brief Height of the top UI bar. Taller to fit input boxes.
     */
    float topBarHeight{60.0f};

    /**
     * @brief Width of the right-hand graph panel.
     */
    float graphPanelWidth{400.0f};

    /**
     * @brief Padding around all UI elements.
     */
    float padding{20.0f};

    /**
     * @brief The calculated bounds of the simulation area.
     */
    sf::FloatRect playgroundBounds;

    // =========================================
    // 4. DARK THEME COLORS
    // =========================================
    sf::Color windowBgColor{30, 30, 30};     // Dark Grey
    sf::Color playgroundBgColor{10, 10, 15}; // Very Dark Blue-Black
    sf::Color guiBgColor{45, 45, 50};        // UI Background
    sf::Color graphBgColor{20, 20, 20};      // Graph Background
    sf::Color textColor{240, 240, 240};      // Off-white text
    sf::Color axisColor{150, 150, 150};    // Light grey for axes
    sf::Color inputBgColor{70, 70, 80};      // Input box background

    /**
     * @brief Constructor.
     * Calculates dynamic layout values like the playground bounds.
     */
    Config()
    {
        // Calculate the simulation area bounds based on other UI elements.
        playgroundBounds = sf::FloatRect(
            {padding, topBarHeight + padding}, // Position {x, y}
            {
                windowWidth - graphPanelWidth - (3 * padding), // Size {width}
                windowHeight - topBarHeight - (2 * padding)  // Size {height}
            });
    }
};