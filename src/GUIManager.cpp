#include "GUIManager.h"
#include "Config.h" // Needs Config for colors and layout
#include <iomanip> // For std::setprecision
#include <sstream> // For std::stringstream
#include <iostream> // FIX: Added for std::cerr

GUIManager::GUIManager(Config& config, sf::Font& font)
    : m_config(config),
      m_font(font),
      m_topBar(),
      m_focusedInput(nullptr),
      m_btnIdleColor(70, 130, 180),  // Steel Blue
      m_btnHoverColor(220, 60, 60) // Crimson Red
{
    // Configure the top bar background
    m_topBar.setPosition({0.f, 0.f});
    m_topBar.setSize({(float)m_config.windowWidth, m_config.topBarHeight});
    m_topBar.setFillColor(m_config.guiBgColor);
    m_topBar.setOutlineColor(sf::Color(10, 10, 10));
    m_topBar.setOutlineThickness(1.0f);

    // --- Create UI Elements ---
    float startX = m_config.padding;
    float btnY = m_config.topBarHeight / 2.0f;

    // Create Buttons
    m_buttons[ButtonType::PauseResume] = createButton("START", {startX + 60, btnY});
    m_buttons[ButtonType::Reset] = createButton("RESET", {startX + 180, btnY});

    // Create Writable Input Boxes
    float inputX = startX + 320;
    float spacing = 160.0f;
    // The GUI directly reads from and writes to the m_config object
    createInput("Spawn:", m_config.initialSpawnRate, {inputX, btnY});
    createInput("Rep:", m_config.initialReplicationRate, {inputX + spacing, btnY});
    createInput("Death:", m_config.initialDeathRate, {inputX + spacing * 2, btnY});
    createInput("Mut:", m_config.initialMutationRate, {inputX + spacing * 3, btnY});

    // Set initial button colors
    for (auto& pair : m_buttons)
    {
        updateButtonColors(pair.second);
    }
}

std::optional<GUIManager::ButtonType> GUIManager::handleClick(const sf::Event& event, const sf::Vector2f& mousePos)
{
    // Check for a left mouse click
    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
        {
            // Check if a control button was clicked
            for (auto& pair : m_buttons)
            {
                if (pair.second.shape.getGlobalBounds().contains(mousePos))
                {
                    // Unfocus any input box
                    if (m_focusedInput)
                    {
                        commitInputValue(*m_focusedInput);
                        m_focusedInput->isFocused = false;
                        m_focusedInput = nullptr;
                    }
                    return pair.first; // Return the button type
                }
            }

            // Check if an input box was clicked
            m_focusedInput = nullptr; // Clear focus first
            for (auto& input : m_inputs)
            {
                if (input.boxShape.getGlobalBounds().contains(mousePos))
                {
                    // Focus this input box
                    m_focusedInput = &input;
                    input.isFocused = true;
                    // Start with an empty string for typing
                    input.currentString = "";
                    input.valueText.setString(input.currentString + "_");
                }
                else
                {
                    // Clicked away from this box, commit and unfocus
                    if (input.isFocused)
                    {
                        commitInputValue(input);
                    }
                    input.isFocused = false;
                }
            }
        }
    }
    return std::nullopt; // No control button was clicked
}

void GUIManager::handleTextEntered(std::uint32_t unicode)
{
    // Only process text if an input box is focused
    if (!m_focusedInput) return;

    // Handle Backspace
    if (unicode == 8) // ASCII for Backspace
    {
        if (!m_focusedInput->currentString.empty())
        {
            m_focusedInput->currentString.pop_back();
        }
    }
    // Handle valid number characters (digits 0-9 and '.')
    else if ((unicode >= '0' && unicode <= '9') || unicode == '.')
    {
        // Limit string length
        if (m_focusedInput->currentString.length() < 6)
        {
            // Allow only one decimal point
            if (unicode != '.' || m_focusedInput->currentString.find('.') == std::string::npos)
            {
                m_focusedInput->currentString += static_cast<char>(unicode);
            }
        }
    }

    // Update the text display with a cursor
    m_focusedInput->valueText.setString(m_focusedInput->currentString + "_");
}

void GUIManager::handleKeyPressed(sf::Keyboard::Key key)
{
    // Only process if an input box is focused
    if (!m_focusedInput) return;

    // Commit and unfocus on 'Enter' or 'Escape'
    if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Escape)
    {
        commitInputValue(*m_focusedInput);
        m_focusedInput->isFocused = false;
        m_focusedInput = nullptr;
    }
}

void GUIManager::update(const sf::Vector2f& mousePos, bool isPaused)
{
    // Update button hover states
    for (auto& pair : m_buttons)
    {
        bool wasHovered = pair.second.isHovered;
        pair.second.isHovered = pair.second.shape.getGlobalBounds().contains(mousePos);

        // Only update colors if the state changed
        if (wasHovered != pair.second.isHovered)
        {
            updateButtonColors(pair.second);
        }
    }

    // Update input box outlines and text (if not focused)
    for (auto& input : m_inputs)
    {
        if (input.isFocused)
        {
            // Focused: white outline
            input.boxShape.setOutlineColor(sf::Color::White);
        }
        else
        {
            // Not Focused: dim outline
            input.boxShape.setOutlineColor(sf::Color(100, 100, 100));
            // Ensure text shows the *actual* value
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3) << (*input.targetValue);
            input.valueText.setString(ss.str());
        }
    }
}

void GUIManager::draw(sf::RenderWindow& window)
{
    // Draw the top bar background
    window.draw(m_topBar);

    // Draw all buttons
    for (const auto& pair : m_buttons)
    {
        window.draw(pair.second.shape);
        if (pair.second.text.has_value())
        {
            window.draw(*pair.second.text);
        }
    }

    // Draw all input boxes
    for (const auto& input : m_inputs)
    {
        window.draw(input.labelText);
        window.draw(input.boxShape);
        window.draw(input.valueText);
    }
}

void GUIManager::reset(bool isPaused)
{
    // Update the text of the main control button
    if (m_buttons[ButtonType::PauseResume].text.has_value())
    {
        m_buttons[ButtonType::PauseResume].text->setString(isPaused ? "START" : "PAUSE");
    }

    // Unfocus any active input box
    if (m_focusedInput)
    {
        commitInputValue(*m_focusedInput);
        m_focusedInput->isFocused = false;
        m_focusedInput = nullptr;
    }
}

// --- Private Helper Functions ---

GUIManager::Button GUIManager::createButton(const std::string& str, sf::Vector2f pos)
{
    Button button;

    // Create text
    button.text.emplace(m_font, str, 18); // emplace constructs sf::Text in place
    button.text->setFillColor(m_config.textColor);
    button.text->setStyle(sf::Text::Style::Bold);

    // Create shape
    sf::FloatRect textBounds = button.text->getLocalBounds();
    button.shape.setSize({textBounds.size.x + 30.f, 30.f});
    button.shape.setOrigin({button.shape.getSize().x / 2.f, button.shape.getSize().y / 2.f});
    button.shape.setPosition(pos);

    // Center text on shape
    button.text->setOrigin({textBounds.position.x + textBounds.size.x / 2.f,
                            textBounds.position.y + textBounds.size.y / 2.f});
    button.text->setPosition(pos);

    return button;
}

void GUIManager::createInput(const std::string& label, float& targetVal, sf::Vector2f pos)
{
    // FIX: Use the new InputBox constructor that takes the font
    m_inputs.emplace_back(m_font, label, &targetVal);
    InputBox& box = m_inputs.back(); // Get a reference to the newly created box

    // Create label text
    box.labelText.setFillColor(m_config.axisColor); // Use axis color for dim label
    box.labelText.setOrigin({0.f, box.labelText.getLocalBounds().size.y / 2.f});
    box.labelText.setPosition(pos);

    // Create input box shape
    float xPos = pos.x + box.labelText.getGlobalBounds().size.x + 8;
    box.boxShape.setSize({80.f, 26.f});
    box.boxShape.setOrigin({0.f, box.boxShape.getSize().y / 2.f});
    box.boxShape.setPosition({xPos, pos.y});
    box.boxShape.setFillColor(m_config.inputBgColor);
    box.boxShape.setOutlineColor(sf::Color(100, 100, 100));
    box.boxShape.setOutlineThickness(1.0f);

    // Create value text
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << targetVal;
    box.currentString = ss.str();
    box.valueText.setString(box.currentString);
    box.valueText.setFillColor(sf::Color::White);
    box.valueText.setStyle(sf::Text::Style::Bold);
    // Center text vertically in box
    box.valueText.setOrigin({0.f, box.valueText.getLocalBounds().size.y / 2.f + box.valueText.getLocalBounds().position.y});
    box.valueText.setPosition({xPos + 5, pos.y}); // 5px padding
}

void GUIManager::updateButtonColors(Button& button)
{
    button.shape.setFillColor(button.isHovered ? m_btnHoverColor : m_btnIdleColor);
}

void GUIManager::commitInputValue(InputBox& input)
{
    // Try to convert the typed string to a float
    try
    {
        // Only update if the string is not empty
        if (!input.currentString.empty())
        {
            float newVal = std::stof(input.currentString);
            *input.targetValue = newVal; // Update the value in Config
        }
    }
    catch (const std::exception& e)
    {
        // On invalid input (e.g., "1.2.3"), just ignore and revert
        std::cerr << "Invalid input: " << input.currentString << std::endl;
    }

    // Reset the text to display the (potentially unchanged) formatted value
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << (*input.targetValue);
    input.valueText.setString(ss.str());
    input.currentString = ss.str();
}