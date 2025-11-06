#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

// Forward-declare
class Config;

/**
 * @brief Defines the inheritable, mutable genetic traits of a species.
 *
 * This struct is used as the key in the 'gene tree' (std::map) to
 * identify unique species. Spawn rate is NOT included as it is a
 * special property of the initial species, not an inheritable gene.
 */
struct Genes
{
    float replicationRate;
    float deathRate;
    float mutationRate;

    /**
     * @brief Comparison operator for std::map.
     *
     * This allows the `Genes` struct to be used as a key in std::map.
     * It provides a consistent way to sort and compare different gene sets.
     */
    bool operator<(const Genes& other) const
    {
        if (replicationRate != other.replicationRate)
            return replicationRate < other.replicationRate;
        if (deathRate != other.deathRate)
            return deathRate < other.deathRate;
        return mutationRate < other.mutationRate;
    }
};

/**
 * @brief Stores all data associated with a single species.
 *
 * This is the 'value' in the species database (std::unordered_map).
 * It links a species ID to its genetics, color, and graph history.
 */
struct SpeciesData
{
    int id;
    Genes genes;
    sf::Color color;
    std::string name;
    float birthTime{0.0f}; // The simulation time when this species first appeared
    bool isExtinct{false}; // Flag to stop processing/drawing if population is 0

    /**
     * @brief Stores the population history as points.
     * sf::Vector2f.x = time in seconds
     * sf::Vector2f.y = population count
     */
    std::vector<sf::Vector2f> populationHistory;
};

/**
 * @brief Manages all species data and the creation of new species.
 *
 * This class is the "brain" of the evolution. It holds two main data structures:
 * 1. An unordered_map (m_speciesDB) for fast ID-to-Data lookups.
 * 2. A map (m_geneLookupTree) for fast Gene-to-ID lookups, which
 * prevents creating duplicate species.
 */
class SpeciesManager
{
public:
    /**
     * @brief Constructor.
     */
    SpeciesManager();

    /**
     * @brief Creates the initial "Species 0" based on config settings.
     * @param config The global simulation configuration.
     * @param currentTime The simulation time (should be 0.0).
     * @return The speciesID of the newly created initial species (always 0).
     */
    int createInitialSpecies(const Config& config, float currentTime);

    /**
     * @brief Attempts to find a species by its genes; creates a new one if not found.
     *
     * This is the core evolution logic. It uses the 'm_geneLookupTree' (std::map)
     * to see if a set of genes already exists.
     *
     * @param newGenes The set of genes to look for (from a mutation).
     * @param config The global simulation configuration.
     * @param currentTime The current simulation time (becomes the species' birthTime).
     * @return The speciesID (either existing or new).
     */
    int findOrCreateSpecies(const Genes& newGenes, const Config& config, float currentTime);

    /**
     * @brief Applies random mutation to a parent's genes.
     * @param parentGenes The genes of the parent organism.
     * @param config The global simulation configuration.
     * @return A new, possibly mutated, set of Genes.
     */
    Genes mutate(const Genes& parentGenes, const Config& config);

    /**
     * @brief Records the current population counts for all species.
     *
     * This function is called periodically by the simulation to log
     * the data needed for the live graph.
     *
     * @param populationCounts A map of <speciesID, count> for all living organisms.
     * @param currentTime The current simulation time for the history timestamp.
     */
    void updatePopulationHistory(const std::map<int, int>& populationCounts, float currentTime);

    /**
     * @brief Resets the manager, clearing all species and history.
     */
    void reset();

    /**
     * @brief Gets a const reference to a species' data (e.g., for drawing).
     * @param speciesID The ID to look up.
     * @return A const reference to the SpeciesData.
     */
    const SpeciesData& getSpeciesData(int speciesID) const;

    /**
     * @brief Gets a const reference to the entire species database (for the graph).
     * @return A const reference to the species database HashMap.
     */
    const std::unordered_map<int, SpeciesData>& getAllSpeciesData() const;

private:
    /**
     * @brief Generates a bright, saturated, and visually distinct random color.
     * @return An sf::Color object.
     */
    sf::Color generateRandomColor();

    /**
     * @brief The main "database" (HashMap). Maps ID -> SpeciesData.
     * Fast O(1) average lookup by ID.
     */
    std::unordered_map<int, SpeciesData> m_speciesDB;

    /**
     * @brief The "gene lookup" (Tree). Maps Genes -> ID.
     * This is used to check if a mutated organism belongs to an
     * already-discovered species.
     * Fast O(log N) lookup by Genes.
     */
    std::map<Genes, int> m_geneLookupTree;

    /**
     * @brief A counter to ensure all new species get a unique ID.
     */
    int m_nextSpeciesID;

    /**
* @brief A default 'null' species data to return on failed lookups.
*/
    SpeciesData m_nullSpecies;
};