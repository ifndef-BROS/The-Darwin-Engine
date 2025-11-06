#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory> // For std::unique_ptr

// --- FIX: Include full definition for Organism ---
// std::vector<Organism> requires the complete type (size) of Organism.
// A forward declaration is not sufficient.
#include "Organism.h"

// Forward-declare other classes that are only used as references
class SpeciesManager;
class Config;

/**
 * @brief Manages the simulation area and all living organisms.
 *
 * This class holds the primary std::vector of all active organisms.
 * It is responsible for updating, drawing, and managing the lifecycle
 * (spawning, death, replication) of all organisms within the
 * defined playground boundaries. It also manages the simulation clock.
 */
class SimulationWorld
{
public:
    /**
     * @brief Constructor.
     * @param config The global simulation configuration.
     * @param speciesManager A reference to the species manager.
     */
    SimulationWorld(const Config& config, SpeciesManager& speciesManager);

    /**
     * @brief Updates the entire simulation state for one frame.
     *
     * This is the core simulation loop. It iterates through all organisms,
     * processes their deaths and replications, and adds new organisms.
     * It also handles the spontaneous spawning of Species 0.
     *
     * @param dt Delta time (time since the last frame).
     */
    void update(float dt);

    /**
     * @brief Draws all elements of the simulation world (playground box and organisms).
     * @param window The SFML render window to draw to.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Resets the simulation to its initial state.
     *
     * Clears all organisms, resets the clock, and spawns the initial population.
     */
    void reset();

    /**
     * @brief Gets the current population counts for all species.
     * @return A map of <speciesID, count> for the SpeciesManager.
     */
    std::map<int, int> getPopulationCounts() const;

    /**
     * @brief Gets the total elapsed simulation time.
     * @return The simulation time in seconds.
     */
    float getTime() const { return m_simulationTime; }

private:
    /**
     * @brief Spawns the initial population (defined in Config).
     */
    void spawnInitialPopulation();

    /**
     * @brief Spawns a single new organism at a specific position.
     * @param speciesID The speciesID of the new organism.
     * @param position The position to spawn at.
     */
    void spawnOrganism(int speciesID, sf::Vector2f position);

    /**
     * @brief Gets a random position within the playground bounds.
     * @return A random sf::Vector2f.
     */
    sf::Vector2f getRandomPlaygroundPos();

    // --- Core Data Structures ---
    /**
     * @brief The main vector holding all living organisms.
     * This requires the full definition of Organism.h.
     */
    std::vector<Organism> m_organisms;

    // --- References ---
    const Config& m_config;
    SpeciesManager& m_speciesManager;

    // --- Graphics ---
    sf::RectangleShape m_playgroundBox; // The black-bordered "playground"

    // --- State & Timing ---
    float m_simulationTime; // Total time elapsed in seconds
    float m_spawnTimer;     // Timer for handling Species 0 spawn rate
};