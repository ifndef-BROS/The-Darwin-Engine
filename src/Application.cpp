#include "Application.h"
#include <stdexcept>
#include <iostream>
#include <optional>

Application::Application()
    : m_config(), // Initialize Config (member-by-value)
      // FIX: sf::VideoMode constructor takes an explicit sf::Vector2u
      m_window(sf::VideoMode(sf::Vector2u{m_config.windowWidth, m_config.windowHeight}), m_config.windowTitle),
      m_font(), // Default-construct the font
      m_isPaused(true) // Start the simulation paused
{
    m_window.setFramerateLimit(m_config.framerateLimit);
    loadAssets(); // Load font, throw error on failure

    // --- Create Manager Objects ---
    // Managers are created in order of dependency
    m_speciesManager = std::make_unique<SpeciesManager>();
    m_world = std::make_unique<SimulationWorld>(m_config, *m_speciesManager);

    // GUI needs a non-const config to edit parameters, and the font
    m_gui = std::make_unique<GUIManager>(m_config, m_font);

    // Graph needs config, species data, and the font
    m_graph = std::make_unique<GraphRenderer>(m_config, *m_speciesManager, m_font);

    // Set the initial GUI state
    resetSimulation();
}

// FIX: Define the destructor in the .cpp file.
// This is crucial for std::unique_ptr members whose
// full definitions were not known in the .h file.
Application::~Application() = default;

void Application::loadAssets()
{
    // Load the font from the assets directory
    if (!m_font.openFromFile("assets/Inter-Regular.ttf"))
    {
        // Throw a critical error if the font is missing
        throw std::runtime_error("Failed to load font: assets/Inter-Regular.ttf. Ensure the 'assets' folder is next to the executable.");
    }
}

void Application::run()
{
    // This is the main application loop
    while (m_window.isOpen())
    {
        // Calculate time elapsed since the last frame
        float dt = m_clock.restart().asSeconds();

        // 1. Handle user input
        processEvents();

        // 2. Update the simulation state
        update(dt);

        // 3. Render the graphics
        render();
    }
}

void Application::processEvents()
{
    // Poll all events that occurred in this frame
    while (const auto event = m_window.pollEvent())
    {
        // --- Window Close Event ---
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }

        // --- Text Input Event ---
        if (const auto* textEvent = event->getIf<sf::Event::TextEntered>())
        {
            // Forward typing to the GUI manager
            m_gui->handleTextEntered(textEvent->unicode);
        }

        // --- Key Press Event ---
        if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
        {
            // Forward keys like 'Enter' or 'Escape' to the GUI
            m_gui->handleKeyPressed(keyEvent->code);
        }

        // --- Mouse Click Event ---
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));
        std::optional<GUIManager::ButtonType> clickedButton = m_gui->handleClick(*event, mousePos);

        if (clickedButton)
        {
            // A control button was clicked
            switch (*clickedButton)
            {
            case GUIManager::ButtonType::PauseResume:
                m_isPaused = !m_isPaused; // Toggle pause state
                m_gui->reset(m_isPaused);
                break;
            case GUIManager::ButtonType::Reset:
                resetSimulation();
                break;
            }
        }
    }
}

void Application::update(float dt)
{
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));

    // Always update the GUI for hover effects and text input
    m_gui->update(mousePos, m_isPaused);

    // If the simulation is paused, do not update the world or graph
    if (m_isPaused)
    {
        // Restart the clocks so delta time doesn't accumulate
        m_clock.restart();
        m_graphUpdateClock.restart();
        return;
    }

    // --- Simulation is Running ---

    // 1. Update the simulation world
    m_world->update(dt);

    // 2. Check if it's time to log data for the graph
    if (m_graphUpdateClock.getElapsedTime().asSeconds() >= m_config.graphUpdateInterval)
    {
        m_graphUpdateClock.restart();
        // Get the latest counts and send them to the SpeciesManager
        // Pass the current time to timestamp the data
        m_speciesManager->updatePopulationHistory(m_world->getPopulationCounts(), m_world->getTime());
    }

    // 3. Update the graph's internal state (max time, max pop)
    m_graph->update(m_world->getTime());
}

void Application::render()
{
    // 1. Clear the window with the theme background color
    m_window.clear(m_config.windowBgColor);

    // 2. Draw all components in order (back to front)
    m_world->draw(m_window);
    m_graph->draw(m_window);
    m_gui->draw(m_window);

    // 3. Display the rendered frame
    m_window.display();
}

void Application::resetSimulation()
{
    // Reset all managers to their initial state
    m_speciesManager->reset();
    m_world->reset();
    m_graph->reset();

    // Set pause state and update GUI
    m_isPaused = true;
    m_gui->reset(m_isPaused);

    // Reset clocks
    m_graphUpdateClock.restart();
    m_clock.restart();
}