#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

// --- FIX: Include full definitions instead of forward declarations ---
// This is required because m_config is a member-by-value, and the
// std::unique_ptrs need the full definitions to know how to call their destructors.
#include "Config.h"
#include "SimulationWorld.h"
#include "SpeciesManager.h"
#include "GUIManager.h"
#include "GraphRenderer.h"

/**
 * @brief The main application class for TheDarwinEngine.
 *
 * This class owns all major components: the render window, the configuration,
 * and all manager objects. It is responsible for running the main game loop
 * and coordinating all updates, events, and rendering.
 */
class Application
{
public:
    /**
     * @brief Constructor.
     * Initializes the window, loads assets, and creates all manager objects.
     */
    Application();

    /**
     * @brief Destructor.
     * Needs to be defined in the .cpp file to handle incomplete types
     * with std::unique_ptr if we were using the PImpl idiom.
     * Since we #include headers, this is fine, but it's good practice.
     */
    ~Application();

    /**
     * @brief Starts and runs the main application loop.
     */
    void run();

private:
    /**
     * @brief Processes all pending SFML window events (e.g., close, resize, input).
     */
    void processEvents();

    /**
     * @brief Updates the state of the simulation and GUI.
     * @param dt Delta time (time since the last frame).
     */
    void update(float dt);

    /**
     * @brief Renders all visible components to the window.
     */
    void render();

    /**
     * @brief Loads required assets (like the font).
     * Throws std::runtime_error on failure.
     */
    void loadAssets();

    /**
     * @brief Resets the entire simulation to its initial state.
     */
    void resetSimulation();

    // --- Core Components ---

    /**
     * @brief Non-const Config so the GUI can write new parameter values to it.
     * m_config is a member-by-value, requiring the full include for Config.h.
     */
    Config m_config;

    /**
     * @brief The main SFML window.
     */
    sf::RenderWindow m_window;

    /**
     * @brief The global font, loaded once and shared via references.
     */
    sf::Font m_font;

    // --- Managers (Owned by Application) ---
    // These require the full class definitions to be known
    // for std::unique_ptr's destructor.
    std::unique_ptr<SpeciesManager> m_speciesManager;
    std::unique_ptr<SimulationWorld> m_world;
    std::unique_ptr<GUIManager> m_gui;
    std::unique_ptr<GraphRenderer> m_graph;

    // --- State & Timing ---
    sf::Clock m_clock;            // SFML clock for delta time
    sf::Clock m_graphUpdateClock; // Clock to time graph data logging
    bool m_isPaused;              // Simulation pause state
};