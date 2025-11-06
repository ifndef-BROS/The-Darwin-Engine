#include "Organism.h"
#include "Config.h"
#include "SpeciesManager.h" // For SpeciesData
#include <cmath>           // For std::cos, std::sin

// This function must be defined in another .cpp file (e.g., SpeciesManager.cpp)
// Using 'extern' links to it.
extern float randomFloat(float min, float max);

Organism::Organism(int id, sf::Vector2f pos, const Config& config)
    : m_speciesID(id),
      m_replicationTimer(0.0f),
      m_deathTimer(0.0f)
{
    // Configure the visual shape
    m_shape.setRadius(config.organismRadius);
    m_shape.setOrigin({config.organismRadius, config.organismRadius});
    m_shape.setPosition(pos);

    // Set an initial random movement vector
    setRandomVelocity(config.organismSpeed);
}

int Organism::update(float dt, const Config& config, const SpeciesData& speciesData, int totalPopulation)
{
    // --- 1. Movement ---
    m_shape.move(m_velocity * dt);
    checkWallCollision(config.playgroundBounds);

    // --- 2. Death Logic ---
    // Timers are used to check rates "per second" rather than "per frame".
    m_deathTimer += dt;
    if (m_deathTimer >= 1.0f)
    {
        m_deathTimer -= 1.0f; // Reset timer for the next second
        if (randomFloat(0.0f, 1.0f) < speciesData.genes.deathRate)
        {
            return -1; // Tell the world we died
        }
    }

    // --- 3. Replication Logic ---
    m_replicationTimer += dt;
    if (m_replicationTimer >= 1.0f)
    {
        m_replicationTimer -= 1.0f; // Reset timer

        // Calculate the "Resource Factor" from the video
        float n = static_cast<float>(totalPopulation);
        float c = static_cast<float>(config.carryingCapacity);
        
        // Ensure factor is between 0 and 1
        float resourceFactor = std::max(0.0f, (1.0f - (n / c)));

        // The *actual* chance to replicate is (Gene Rate * Resource Factor)
        float effectiveReplicationRate = speciesData.genes.replicationRate * resourceFactor;

        if (randomFloat(0.0f, 1.0f) < effectiveReplicationRate)
        {
            return 1; // Tell the world we replicated
        }
    }

    return 0; // Nothing happened
}

void Organism::draw(sf::RenderWindow& window, sf::Color color)
{
    m_shape.setFillColor(color);
    window.draw(m_shape);
}

void Organism::checkWallCollision(const sf::FloatRect& bounds)
{
    sf::Vector2f pos = m_shape.getPosition();
    float radius = m_shape.getRadius();

    // SFML 3.0: Rects use .position and .size
    // Check left wall
    if (pos.x - radius < bounds.position.x)
    {
        pos.x = bounds.position.x + radius;
        m_velocity.x = -m_velocity.x; // Reverse X velocity
    }
    // Check right wall
    else if (pos.x + radius > bounds.position.x + bounds.size.x)
    {
        pos.x = bounds.position.x + bounds.size.x - radius;
        m_velocity.x = -m_velocity.x;
    }

    // Check top wall
    if (pos.y - radius < bounds.position.y)
    {
        pos.y = bounds.position.y + radius;
        m_velocity.y = -m_velocity.y; // Reverse Y velocity
    }
    // Check bottom wall
    else if (pos.y + radius > bounds.position.y + bounds.size.y)
    {
        pos.y = bounds.position.y + bounds.size.y - radius;
        m_velocity.y = -m_velocity.y;
    }

    m_shape.setPosition(pos);
}

void Organism::setRandomVelocity(float speed)
{
    // Generate a random angle in radians
    float angle = randomFloat(0.0f, 2.0f * 3.14159265f);
    m_velocity.x = std::cos(angle) * speed;
    m_velocity.y = std::sin(angle) * speed;
}