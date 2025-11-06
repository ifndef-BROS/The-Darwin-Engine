cmake_minimum_required(VERSION 3.15)

# 1. Project Definition
project(TheDarwinEngine VERSION 1.0 LANGUAGES CXX)

# 2. C++ Standard
# We enforce C++17, which is required by SFML 3.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# 3. Find SFML 3.0
# This finds the SFML package and its components.
# Ensure SFML_DIR is set in your environment or CMake config if not found.
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

# 4. Project Structure
# Add the 'include' directory to the include path.
include_directories(include)

# Find all .cpp files in the 'src' directory.
file(GLOB SOURCES "src/*.cpp")

# 5. Asset Handling
# Copies the 'assets' folder (containing the font) to the build directory
# so the executable can find it at runtime.
set(ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
if(EXISTS "${ASSET_DIR}")
    file(COPY ${ASSET_DIR} DESTINATION ${CMAKE_BINARY_DIR})
    message(STATUS "Assets copied to build directory: ${CMAKE_BINARY_DIR}/assets")
else()
    message(WARNING "Assets directory not found at ${ASSET_DIR}")
endif()

# 6. Executable Target
# Create the executable.
# WIN32: Hides the black console window on Windows.
# MACOSX_BUNDLE: Creates a proper .app bundle on macOS.
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCES})

# 7. Link Libraries
# Link our executable against the SFML 3 targets.
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)

message(STATUS "${PROJECT_NAME} configuration complete.")
