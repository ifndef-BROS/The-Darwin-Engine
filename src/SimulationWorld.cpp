#include "SimulationWorld.h"
#include "Organism.h"       // For creating Organism objects
#include "SpeciesManager.h" // For getting species data
#include "Config.h"         // For playground bounds and sim rules
#include <iostream>        // For std::cerr

// Link to the random function defined in SpeciesManager.cpp
extern float randomFloat(float min, float max);

SimulationWorld::SimulationWorld(const Config& config, SpeciesManager& speciesManager)
    : m_config(config),
      m_speciesManager(speciesManager),
      m_simulationTime(0.0f),
      m_spawnTimer(0.0f)
{
    // Configure the visual appearance of the playground box
    m_playgroundBox.setPosition(m_config.playgroundBounds.position);
    m_playgroundBox.setSize(m_config.playgroundBounds.size);
    m_playgroundBox.setFillColor(m_config.playgroundBgColor);
    m_playgroundBox.setOutlineColor(sf::Color(60, 60, 60));
    m_playgroundBox.setOutlineThickness(2.0f);

    // Spawn the first generation
    spawnInitialPopulation();
}

void SimulationWorld::update(float dt)
{
    // Increment the master simulation clock
    m_simulationTime += dt;

    // Get the total population *before* this frame's updates
    int totalPopulation = m_organisms.size();

    // --- 1. Spontaneous Spawning (Species 0 only) ---
    // This implements the "initialSpawnRate"
    m_spawnTimer += dt;
    if (m_spawnTimer >= 1.0f)
    {
        m_spawnTimer -= 1.0f; // Reset timer for the next second
        // Check if a new Species 0 should be spawned
        if (randomFloat(0.0f, 1.0f) < m_config.initialSpawnRate)
        {
            // Only spawn if we are below the carrying capacity
            if (totalPopulation < m_config.carryingCapacity)
            {
                // We ask the SpeciesManager for ID 0 (or to create it)
                int initialSpeciesID = m_speciesManager.createInitialSpecies(m_config, m_simulationTime);
                spawnOrganism(initialSpeciesID, getRandomPlaygroundPos());
            }
        }
    }

    // --- 2. Main Organism Update Loop ---
    // A temporary vector to store new organisms born this frame
    std::vector<Organism> newOrganisms;

    // We iterate backwards to allow for safe and efficient removal
    for (int i = m_organisms.size() - 1; i >= 0; --i)
    {
        Organism& org = m_organisms[i];
        // Get the genetic data for this organism
        const SpeciesData& species = m_speciesManager.getSpeciesData(org.getSpeciesID());

        // Update the organism and get its status
        int status = org.update(dt, m_config, species, totalPopulation);

        if (status == -1)
        {
            // --- Organism Died ---
            // Swap with the last element and pop_back (fast removal)
            std::swap(m_organisms[i], m_organisms.back());
            m_organisms.pop_back();
        }
        else if (status == 1)
        {
            // --- Organism Replicated ---
            // 1. Get the parent's genes
            const Genes& parentGenes = species.genes;

            // 2. Apply mutation to get the child's genes
            Genes childGenes = m_speciesManager.mutate(parentGenes, m_config);

            // 3. Find or create the species for this child
            int childSpeciesID = m_speciesManager.findOrCreateSpecies(childGenes, m_config, m_simulationTime);

            // 4. Add the new organism to the temporary "newborns" list
            newOrganisms.push_back(Organism(childSpeciesID, org.getPosition(), m_config));
        }
    }

    // --- 3. Add Newborns ---
    // Add all the new organisms from the temporary list to the main vector
    for (const auto& newOrg : newOrganisms)
    {
        // Only add if we are not (too far) over capacity
        if (m_organisms.size() < m_config.carryingCapacity * 1.2) // 20% buffer
        {
            m_organisms.push_back(newOrg);
        }
    }
}

void SimulationWorld::draw(sf::RenderWindow& window)
{
    // 1. Draw the dark playground box
    window.draw(m_playgroundBox);

    // 2. Draw all the organisms
    for (auto& org : m_organisms)
    {
        // Get the color for this organism's species
        const SpeciesData& species = m_speciesManager.getSpeciesData(org.getSpeciesID());
        org.draw(window, species.color);
    }
}

void SimulationWorld::reset()
{
    // Clear the vector and reset the clocks
    m_organisms.clear();
    m_simulationTime = 0.0f;
    m_spawnTimer = 0.0f;

    // Respawn the initial population
    spawnInitialPopulation();
}

std::map<int, int> SimulationWorld::getPopulationCounts() const
{
    std::map<int, int> counts;
    // Iterate through all living organisms and count them by species
    for (const auto& org : m_organisms)
    {
        counts[org.getSpeciesID()]++;
    }
    return counts;
}

void SimulationWorld::spawnInitialPopulation()
{
    // Get the ID of the first "Species 0"
    int initialSpeciesID = m_speciesManager.createInitialSpecies(m_config, 0.0f);

    for (int i = 0; i < m_config.initialPopulation; ++i)
    {
        spawnOrganism(initialSpeciesID, getRandomPlaygroundPos());
    }
}

void SimulationWorld::spawnOrganism(int speciesID, sf::Vector2f position)
{
    // Add a new organism to the main vector
    m_organisms.emplace_back(speciesID, position, m_config);
}

sf::Vector2f SimulationWorld::getRandomPlaygroundPos()
{
    // Generate a random (x, y) position within the playground
    return sf::Vector2f(
        randomFloat(m_config.playgroundBounds.position.x,
                    m_config.playgroundBounds.position.x + m_config.playgroundBounds.size.x),
        randomFloat(m_config.playgroundBounds.position.y,
                    m_config.playgroundBounds.position.y + m_config.playgroundBounds.size.y)
    );
}