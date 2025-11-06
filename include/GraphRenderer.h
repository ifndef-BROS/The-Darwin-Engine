#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory> // Added for consistency, though not strictly needed by this header

// Forward-declare
class Config;
class SpeciesManager;

/**
 * @brief Handles all drawing for the dynamic, scaling population graph.
 *
 * This class reads data from the SpeciesManager and renders it as a
 * time-series line graph. It manages the graph's coordinate system,
 * dynamic axes with scaling labels, and a "Max Population" display.
 */
class GraphRenderer
{
public:
    /**
     * @brief Constructor.
     * @param config The global simulation configuration.
     * @param speciesManager A reference to the species manager.
     * @param font A reference to the loaded global font.
     */
    GraphRenderer(const Config& config, SpeciesManager& speciesManager, sf::Font& font);

    /**
     * @brief Updates the graph's internal state (current time, max pop).
     * This is called every frame before drawing.
     * @param currentTime The total elapsed simulation time in seconds.
     */
    void update(float currentTime);

    /**
     * @brief Draws the entire graph component to the window.
     * @param window The render window.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Resets the graph's history and max values.
     */
    void reset();

private:
    /**
     * @brief Draws the graph axes, gridlines, and scaling labels.
     * @param window The render window.
     */
    void drawAxes(sf::RenderWindow& window);

    /**
     * @brief Draws the population lines for all species, respecting their birthTime.
     * @param window The render window.
     */
    void drawLines(sf::RenderWindow& window);

    /**
     * @brief Helper to get a "nice" rounded number for the top of an axis.
     * @param value The raw maximum value (e.g., 1382).
     * @return A rounded value (e.g., 1500).
     */
    float getNiceAxisMax(float value);

    /**
     * @brief Helper to format time in seconds to a string (e.g., "1.5m").
     * @param seconds The time in seconds.
     * @return A formatted string.
     */
    std::string formatTime(float seconds);

    // --- References ---
    const Config& m_config;
    SpeciesManager& m_speciesManager;
    sf::Font& m_font;

    // --- Layout ---
    sf::RectangleShape m_background;
    sf::FloatRect m_plotArea; // The inner area where lines are drawn

    // --- "Max Pop" Box UI ---
    sf::Text m_maxPopLabelText; // "MAX POP EVER"
    sf::Text m_maxPopValueText; // "1234"

    // --- Dynamic State ---
    float m_currentTime;       // Current simulation time
    float m_currentMaxPop;     // Current max Y-axis value (for scaling)
    float m_currentMaxTime;    // Current max X-axis value (for scaling)
    int m_maxPopulationEver; // All-time highest population
};