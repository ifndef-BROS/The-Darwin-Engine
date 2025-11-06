#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <optional>
#include <vector>

// Forward-declare
class Config;

/**
 * @brief Manages all GUI elements: buttons and parameter text inputs.
 *
 * This class handles drawing and all user interaction with the top UI bar.
 * It manages button clicks, text box focusing, and keyboard input
 * to modify simulation parameters in the Config object.
 */
class GUIManager
{
public:
    /**
     * @brief Identifiers for the main control buttons.
     */
    enum class ButtonType
    {
        PauseResume,
        Reset
    };

    /**
     * @brief Constructor.
     * @param config A non-const reference to the global config, allowing
     * the GUI to change parameter values.
     * @param font A reference to the loaded global font.
     */
    GUIManager(Config& config, sf::Font& font);

    /**
     * @brief Handles mouse click events for buttons and input boxes.
     * @param event The SFML event to process.
     * @param mousePos Current window mouse position.
     * @return An optional ButtonType if a control button was clicked.
     */
    std::optional<ButtonType> handleClick(const sf::Event& event, const sf::Vector2f& mousePos);

    /**
     * @brief Handles incoming text events (typing).
     * @param unicode The character code entered.
     */
    void handleTextEntered(std::uint32_t unicode);

    /**
     * @brief Handles non-text key presses (e.g., Enter, Escape).
     * @param key The key code pressed.
     */
    void handleKeyPressed(sf::Keyboard::Key key);

    /**
     * @brief Updates hover states and button text.
     * @param mousePos Current window mouse position.
     * @param isPaused The simulation's pause state.
     */
    void update(const sf::Vector2f& mousePos, bool isPaused);

    /**
     * @brief Draws all GUI elements to the window.
     * @param window The render window.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Resets button text and unfocuses input boxes.
     * @param isPaused The simulation's pause state.
     */
    void reset(bool isPaused);

private:
    /**
     * @brief Holds all components for a single clickable button.
     */
    struct Button
    {
        sf::RectangleShape shape;
        std::optional<sf::Text> text; // Optional for SFML 3 compatibility
        bool isHovered{false};
    };

    /**
     * @brief Holds all components for a single parameter input box.
     */
    struct InputBox
    {
        // --- FIX: Initialize sf::Text members in a constructor ---
        /**
         * @brief Constructs an InputBox.
         * @param font The global font.
         * @param label The label string.
         * @param target Pointer to the float value in Config.
         */
        InputBox(sf::Font& font, const std::string& label, float* target)
            : labelText(font), // Initialize sf::Text with font
              valueText(font), // Initialize sf::Text with font
              labelStr(label),
              isFocused(false),
              targetValue(target)
        {
        }

        std::string labelStr;
        sf::Text labelText;
        sf::RectangleShape boxShape;
        sf::Text valueText;
        std::string currentString;    // Temporary string while typing
        bool isFocused{false};
        float* targetValue{nullptr}; // Pointer to the Config value this box edits
    };

    /**
     * @brief Factory function to create a new Button.
     * @param str The text for the button.
     * @param pos The center position of the button.
     * @return A configured Button object.
     */
    Button createButton(const std::string& str, sf::Vector2f pos);

    /**
     * @brief Factory function to create and register a new InputBox.
     * @param label The descriptive label (e.g., "Spawn:").
     * @param targetVal A reference to the float value in Config to modify.
     * @param pos The top-left position of the *label*.
     */
    void createInput(const std::string& label, float& targetVal, sf::Vector2f pos);

    /**
     * @brief Updates a button's colors based on its hover state.
     * @param button The button to update.
     */
    void updateButtonColors(Button& button);

    /**
     * @brief Commits the typed value from an input box to the Config.
     * @param input The input box to commit.
     */
    void commitInputValue(InputBox& input);

    // --- References ---
    Config& m_config;     // Non-const ref to edit parameters
    sf::Font& m_font;

    // --- UI Elements ---
    sf::RectangleShape m_topBar;
    std::map<ButtonType, Button> m_buttons;
    std::vector<InputBox> m_inputs;
    InputBox* m_focusedInput{nullptr}; // Tracks which input box is active

    // --- Theme Colors ---
    sf::Color m_btnIdleColor;
    sf::Color m_btnHoverColor;
};