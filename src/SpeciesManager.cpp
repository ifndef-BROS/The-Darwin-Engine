#include "SpeciesManager.h"
#include "Config.h"
#include <random>    // For std::random_device, std::mt19937
#include <cmath>     // For std::fmod, std::abs
#include <iostream>  // For std::cerr
#include <cstdint>   // For std::uint8_t

/**
 * @brief Utility function to generate a random float within a range.
 * This is defined here and declared 'extern' in other files that need it.
 */
float randomFloat(float min, float max)
{
    // Static ensures these are initialized only once
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

SpeciesManager::SpeciesManager()
    : m_nextSpeciesID(0)
{
    // Initialize the 'null' species to prevent crashes on bad lookups
    m_nullSpecies.id = -1;
    m_nullSpecies.name = "Null";
    m_nullSpecies.color = sf::Color::Magenta;
    m_nullSpecies.genes = {0.f, 0.f, 0.f};
    m_nullSpecies.birthTime = 0.0f;
    m_nullSpecies.isExtinct = true;
}

int SpeciesManager::createInitialSpecies(const Config& config, float currentTime)
{
    // Define the genes for the first species ("Species 0")
    Genes initialGenes;
    initialGenes.replicationRate = config.initialReplicationRate;
    initialGenes.deathRate = config.initialDeathRate;
    initialGenes.mutationRate = config.initialMutationRate;

    // Use findOrCreateSpecies to add it to the database
    // This ensures it gets added to both the HashMap and the Tree
    return findOrCreateSpecies(initialGenes, config, currentTime);
}

int SpeciesManager::findOrCreateSpecies(const Genes& newGenes, const Config& config, float currentTime)
{
    // 1. Search the "Tree" (std::map)
    // Check if this exact set of genes already exists.
    auto it = m_geneLookupTree.find(newGenes);

    // 2. If found, return the existing ID
    if (it != m_geneLookupTree.end())
    {
        // This species already exists, return its ID
        return it->second;
    }

    // 3. If not found, create a new species
    int newID = m_nextSpeciesID++;
    SpeciesData newData;
    newData.id = newID;
    newData.genes = newGenes;
    newData.color = generateRandomColor();
    newData.name = "Species " + std::to_string(newID);
    newData.birthTime = currentTime; // Record the exact time it appeared
    newData.isExtinct = false;

    // Add an initial history point at its birth time with 0 population.
    // This ensures the graph line starts at the correct time.
    newData.populationHistory.push_back({currentTime, 0.f});

    // 3a. Add the new species to the "HashMap" (std::unordered_map)
    m_speciesDB[newID] = newData;

    // 3b. Add the new gene signature to the "Tree" (std::map)
    m_geneLookupTree[newGenes] = newID;

    // std::cout << "New Species Discovered (ID: " << newID << ") at T=" << currentTime << "s\n";

    return newID;
}

Genes SpeciesManager::mutate(const Genes& parentGenes, const Config& config)
{
    Genes childGenes = parentGenes;

    // Helper lambda to mutate a single gene value
    auto mutateGene = [&](float& geneValue)
    {
        float mutation = randomFloat(-config.mutationAmount, config.mutationAmount);
        // Apply mutation and ensure it doesn't go below zero
        geneValue = std::max(0.0f, geneValue + (geneValue * mutation));
    };

    // Only mutate if a random check passes (based on parent's mutation rate)
    if (randomFloat(0.0f, 1.0f) < parentGenes.mutationRate)
    {
        // Mutate all three inheritable genes
        mutateGene(childGenes.replicationRate);
        mutateGene(childGenes.deathRate);
        mutateGene(childGenes.mutationRate);
    }

    return childGenes;
}

void SpeciesManager::updatePopulationHistory(const std::map<int, int>& populationCounts, float currentTime)
{
    // Iterate over our main species database (the HashMap)
    for (auto& pair : m_speciesDB)
    {
        int speciesID = pair.first;
        SpeciesData& species = pair.second;

        // If species is already extinct, don't add more data points
        if (species.isExtinct)
        {
            continue;
        }

        int currentPop = 0;
        auto it = populationCounts.find(speciesID);
        if (it != populationCounts.end())
        {
            currentPop = it->second; // Found living organisms
        }

        // Add the new data point (time, population)
        species.populationHistory.push_back({currentTime, (float)currentPop});

        // Check if the species just went extinct
        if (currentPop == 0 && species.populationHistory.size() > 1)
        {
            // If the last point was also 0, mark as officially extinct
            if (species.populationHistory[species.populationHistory.size() - 2].y == 0.f)
            {
                species.isExtinct = true;
            }
        }
    }
}

void SpeciesManager::reset()
{
    // Clear all data structures
    m_speciesDB.clear();
    m_geneLookupTree.clear();
    m_nextSpeciesID = 0;
}

const SpeciesData& SpeciesManager::getSpeciesData(int speciesID) const
{
    // Look up the species in the "HashMap"
    auto it = m_speciesDB.find(speciesID);
    if (it != m_speciesDB.end())
    {
        return it->second; // Return found data
    }

    // Return a safe "null" object if the ID is invalid
    std::cerr << "Warning: Tried to access invalid speciesID: " << speciesID << "\n";
    return m_nullSpecies;
}

const std::unordered_map<int, SpeciesData>& SpeciesManager::getAllSpeciesData() const
{
    return m_speciesDB;
}

sf::Color SpeciesManager::generateRandomColor()
{
    // Algorithm to generate bright, saturated, "neon" colors
    // by converting from HSV (Hue, Saturation, Value) to RGB.
    float hue = randomFloat(0.0f, 360.0f);
    float saturation = 0.8f; // High saturation
    float value = 1.0f;      // Max brightness

    float c = value * saturation;
    float x = c * (1.0f - std::abs(fmod(hue / 60.0f, 2.0f) - 1.0f));
    float m = value - c;
    float r = 0.f, g = 0.f, b = 0.f;

    if (hue < 60.f) { r = c; g = x; }
    else if (hue < 120.f) { r = x; g = c; }
    else if (hue < 180.f) { g = c; b = x; }
    else if (hue < 240.f) { g = x; b = c; }
    else if (hue < 300.f) { r = x; b = c; }
    else { r = c; b = x; }

    return sf::Color(
        static_cast<std::uint8_t>((r + m) * 255),
        static_cast<std::uint8_t>((g + m) * 255),
        static_cast<std::uint8_t>((b + m) * 255)
    );
}