#include "Application.h" // The main application class
#include <iostream>      // For std::cerr
#include <stdexcept>     // For std::exception

/**
 * @brief Main entry point for TheDarwinEngine
 *
 * This function creates and runs the main Application object.
 * It includes a top-level try-catch block to handle any critical
 * exceptions that might occur during initialization (e.g., font not found).
 */
int main()
{
    try
    {
        // 1. Create the Application object.
        // The Application's constructor handles all setup:
        // creating the window, loading assets, and initializing all managers.
        Application app;

        // 2. Run the Application.
        // This starts the main game loop, which will only exit
        // when the user closes the window.
        app.run();
    }
    catch (const std::exception& e)
    {
        // If anything went critically wrong (e.g., failed to load the font),
        // print the error to the console (or a popup dialog).
        std::cerr << "Critical Error: " << e.what() << std::endl;
        // On Windows, this will still print to the console if launched from one,
        // or can be redirected to a log file.
        return 1; // Exit with an error code
    }

    // 0 = Success
    return 0;
}