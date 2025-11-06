#include "GraphRenderer.h"
#include "Config.h"
#include "SpeciesManager.h" // For SpeciesData
#include <iomanip>  // For std::setprecision
#include <sstream>  // For std::stringstream
#include <cmath>    // For std::pow, std::floor, std::log10

GraphRenderer::GraphRenderer(const Config& config, SpeciesManager& speciesManager, sf::Font& font)
    : m_config(config),
      m_speciesManager(speciesManager),
      m_font(font),
      m_background(),
      m_plotArea(),
      m_maxPopLabelText(m_font),   // Initialize sf::Text members with the font
      m_maxPopValueText(m_font),
      m_currentTime(0.0f),
      m_currentMaxPop(10.0f),  // Start with a small default axis
      m_currentMaxTime(60.0f), // Start with a 60-second axis
      m_maxPopulationEver(0)
{
    // --- Configure Background ---
    float graphX = m_config.playgroundBounds.position.x + m_config.playgroundBounds.size.x + m_config.padding;
    m_background.setPosition({graphX, m_config.topBarHeight + m_config.padding});
    m_background.setSize({m_config.windowWidth - graphX - m_config.padding, m_config.windowHeight - m_config.topBarHeight - (2 * m_config.padding)});
    m_background.setFillColor(m_config.graphBgColor);
    m_background.setOutlineColor(sf::Color(60, 60, 60));
    m_background.setOutlineThickness(1.0f);

    // --- Configure Plot Area ---
    // The inner rect where lines are actually drawn
    m_plotArea = sf::FloatRect(
        {m_background.getPosition().x + 50, m_background.getPosition().y + 40}, // {left, top} with padding for labels
        {m_background.getSize().x - 70, m_background.getSize().y - 80}          // {width, height}
    );

    // --- Configure "Max Pop" Box ---
    m_maxPopLabelText.setString("MAX POP EVER");
    m_maxPopLabelText.setFont(m_font);
    m_maxPopLabelText.setCharacterSize(12);
    m_maxPopLabelText.setFillColor(m_config.axisColor);
    m_maxPopLabelText.setPosition({m_background.getPosition().x + m_background.getSize().x - 110, m_background.getPosition().y + 10});

    m_maxPopValueText.setString("0");
    m_maxPopValueText.setFont(m_font);
    m_maxPopValueText.setCharacterSize(24);
    m_maxPopValueText.setFillColor(m_config.textColor);
    m_maxPopValueText.setStyle(sf::Text::Style::Bold);
    // Position below the label
    m_maxPopValueText.setPosition({m_maxPopLabelText.getPosition().x, m_maxPopLabelText.getPosition().y + 15});
}

void GraphRenderer::update(float currentTime)
{
    m_currentTime = currentTime;

    // --- Update Max Population (for Y-Axis) ---
    float maxPop = 10.0f; // Start with a minimum of 10
    for (const auto& pair : m_speciesManager.getAllSpeciesData())
    {
        // Only check non-extinct species for current max
        if (!pair.second.isExtinct)
        {
            // Check the last recorded population
            if (!pair.second.populationHistory.empty())
            {
                float lastPop = pair.second.populationHistory.back().y;
                if (lastPop > maxPop)
                {
                    maxPop = lastPop;
                }
            }
        }
        // Check all species for all-time max
        for (const auto& point : pair.second.populationHistory)
        {
            if (point.y > m_maxPopulationEver)
            {
                m_maxPopulationEver = static_cast<int>(point.y);
            }
        }
    }
    m_currentMaxPop = getNiceAxisMax(maxPop); // Round to a "nice" number
    m_maxPopValueText.setString(std::to_string(m_maxPopulationEver));

    // --- Update Max Time (for X-Axis) ---
    // Make the time axis always at least 60 seconds, or 110% of current time
    m_currentMaxTime = std::max(60.0f, m_currentTime * 1.1f);
}

void GraphRenderer::draw(sf::RenderWindow& window)
{
    // Draw background and "Max Pop" box
    window.draw(m_background);
    window.draw(m_maxPopLabelText);
    window.draw(m_maxPopValueText);

    // Draw axes, gridlines, and labels
    drawAxes(window);

    // Draw all the species population lines
    drawLines(window);
}

void GraphRenderer::reset()
{
    // Reset all dynamic state
    m_currentTime = 0.0f;
    m_maxPopulationEver = 0;
    m_currentMaxPop = 10.0f;
    m_currentMaxTime = 60.0f;
    m_maxPopValueText.setString("0");
}

void GraphRenderer::drawAxes(sf::RenderWindow& window)
{
    sf::VertexArray lines(sf::PrimitiveType::Lines);
    sf::Color gridColor(80, 80, 80); // Faint gridlines

    // --- Y-Axis (Population) ---
    int numYSteps = 5; // Draw 5 gridlines
    for (int i = 0; i <= numYSteps; ++i)
    {
        float value = m_currentMaxPop * (i / (float)numYSteps);
        float yPos = m_plotArea.position.y + m_plotArea.size.y - (i / (float)numYSteps) * m_plotArea.size.y;

        // Create gridline
        lines.append(sf::Vertex{{m_plotArea.position.x, yPos}, gridColor});
        lines.append(sf::Vertex{{m_plotArea.position.x + m_plotArea.size.x, yPos}, gridColor});

        // Create label
        sf::Text label(m_font, std::to_string(static_cast<int>(value)), 12);
        label.setFillColor(m_config.axisColor);
        // Align text to the right of the axis
        label.setOrigin({label.getLocalBounds().size.x, label.getLocalBounds().size.y / 2.0f});
        label.setPosition({m_plotArea.position.x - 8, yPos});
        window.draw(label);
    }

    // --- X-Axis (Time) ---
    int numXSteps = 6; // Draw 6 time gridlines
    for (int i = 0; i <= numXSteps; ++i)
    {
        float timeVal = m_currentMaxTime * (i / (float)numXSteps);
        float xPos = m_plotArea.position.x + (i / (float)numXSteps) * m_plotArea.size.x;

        // Create gridline
        lines.append(sf::Vertex{{xPos, m_plotArea.position.y}, gridColor});
        lines.append(sf::Vertex{{xPos, m_plotArea.position.y + m_plotArea.size.y}, gridColor});

        // Create label
        sf::Text label(m_font, formatTime(timeVal), 12);
        label.setFillColor(m_config.axisColor);
        label.setOrigin({label.getLocalBounds().size.x / 2.0f, 0.0f});
        label.setPosition({xPos, m_plotArea.position.y + m_plotArea.size.y + 8});
        window.draw(label);
    }

    // Draw all gridlines at once
    window.draw(lines);
}

void GraphRenderer::drawLines(sf::RenderWindow& window)
{
    const auto& allSpecies = m_speciesManager.getAllSpeciesData();

    for (const auto& pair : allSpecies)
    {
        const auto& species = pair.second;
        const auto& history = species.populationHistory;

        // Need at least 2 points to draw a line
        if (history.size() < 2 || species.isExtinct)
        {
            continue;
        }

        sf::VertexArray line(sf::PrimitiveType::LineStrip);

        for (const auto& point : history)
        {
            // point.x = time, point.y = population
            float x = m_plotArea.position.x + (point.x / m_currentMaxTime) * m_plotArea.size.x;
            float y = m_plotArea.position.y + m_plotArea.size.y - (point.y / m_currentMaxPop) * m_plotArea.size.y;

            // Only draw points that are within the graph's visible time
            if (x >= m_plotArea.position.x)
            {
                // Clamp Y-value to prevent line from going below the axis
                y = std::max(y, m_plotArea.position.y);
                y = std::min(y, m_plotArea.position.y + m_plotArea.size.y);
                line.append(sf::Vertex{{x, y}, species.color});
            }
        }
        window.draw(line);
    }
}

float GraphRenderer::getNiceAxisMax(float value)
{
    // Simple algorithm to find a "nice" round number for the top of the axis
    if (value <= 10.f) return 10.f;
    if (value <= 50.f) return 50.f;

    const float power = std::pow(10.f, std::floor(std::log10(value))); // 1382 -> 1000
    const float relative = value / power;                              // 1382 -> 1.382

    if (relative < 1.5f) return 1.5f * power; // 1500
    if (relative < 2.f) return 2.f * power;   // 2000
    if (relative < 3.f) return 3.f * power;   // 3000
    if (relative < 5.f) return 5.f * power;   // 5000
    if (relative < 7.f) return 7.f * power;   // 7000
    return 10.f * power;                      // 10000
}

std::string GraphRenderer::formatTime(float seconds)
{
    std::stringstream ss;
    if (seconds < 60.f)
    {
        ss << static_cast<int>(seconds) << "s";
    }
    else if (seconds < 3600.f)
    {
        ss << std::fixed << std::setprecision(1) << (seconds / 60.f) << "m";
    }
    else
    {
        ss << std::fixed << std::setprecision(1) << (seconds / 3600.f) << "h";
    }
    return ss.str();
}