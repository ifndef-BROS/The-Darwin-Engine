#pragma once

#include <SFML/Graphics.hpp>

// Forward-declare dependencies
class Config;
struct SpeciesData;

/**
 * @brief Represents a single organism ("blob") in the simulation.
 *
 * This class holds the state of one entity on the screen, including its
 * physical properties (position, velocity) and its genetic identity
 * (its speciesID). It does not store its genes directly, only the
 * ID that links to the SpeciesManager.
 */
class Organism
{
public:
    /**
     * @brief Creates a new organism.
     * @param id The speciesID this organism belongs to.
     * @param pos The initial position to spawn at.
     * @param config The global simulation configuration.
     */
    Organism(int id, sf::Vector2f pos, const Config& config);

    /**
     * @brief Updates the organism's state for one frame.
     *
     * This moves the organism, checks for wall collisions, and handles
     * the core evolutionary logic (death, replication) based on its
     * species' genes and the current environment.
     *
     * @param dt Delta time (time since the last frame).
     * @param config The global simulation configuration.
     * @param speciesData The genetic data for this organism's species.
     * @param totalPopulation The current total number of all organisms.
     * @return An integer status:
     * -1 = Organism died this frame.
     * 0 = Nothing happened.
     * 1 = Organism replicated this frame.
     */
    int update(float dt, const Config& config, const SpeciesData& speciesData, int totalPopulation);

    /**
     * @brief Draws the organism to the window.
     * @param window The SFML render window to draw to.
     * @param color The color of this organism's species.
     */
    void draw(sf::RenderWindow& window, sf::Color color);

    /**
     * @brief Gets the species ID of this organism.
     * @return The organism's speciesID.
     */
    int getSpeciesID() const { return m_speciesID; }

    /**
     * @brief Gets the position of this organism.
     * @return The organism's current position vector.
     */
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }

private:
    /**
     * @brief Checks for collision with playground walls and bounces.
     * @param bounds The playground boundary rectangle.
     */
    void checkWallCollision(const sf::FloatRect& bounds);

    /**
     * @brief Sets a new random velocity.
     * @param speed The magnitude of the velocity (speed).
     */
    void setRandomVelocity(float speed);

    int m_speciesID;         // ID linking to the SpeciesManager
    sf::CircleShape m_shape; // The visual representation (a circle)
    sf::Vector2f m_velocity; // Current movement direction and speed

    // Timers to make sure percent-per-second chances are handled correctly
    // (e.g., a 5% chance per second, not per frame)
    float m_replicationTimer;
    float m_deathTimer;
};