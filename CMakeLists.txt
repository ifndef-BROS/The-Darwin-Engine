# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(RLBrainProject VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Boost Configuration ---
# Set the minimum required Boost version
find_package(Boost 1.71.0 REQUIRED)

if(Boost_FOUND)
    message(STATUS "Boost version: ${Boost_VERSION_STRING}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found!")
endif()

# --- Project Structure ---
# Add the 'include' directory to the include path
include_directories(include)

# --- Executable ---
# Add the executable target and its source files
add_executable(rl_brain src/main.cpp src/BrainGraph.cpp)

# --- Linking Libraries ---
# Link the Boost libraries to your executable
# Boost.Graph is mostly header-only, but other components might be needed.
# For this example, no specific components are required for basic graph creation.
# If you use other Boost libraries like program_options or filesystem, you would add them here.
# Example: find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)
# target_link_libraries(rl_brain PRIVATE ${Boost_LIBRARIES})

# If using Boost imported targets (for Boost 1.70+ and modern CMake)
# This is the more modern and recommended approach
# find_package(Boost 1.71.0 REQUIRED)
# target_link_libraries(rl_brain PRIVATE Boost::boost)
