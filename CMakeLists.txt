cmake_minimum_required(VERSION 3.10)
project(EvolutionaryRL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/simulation.cpp
    src/grid.cpp
    src/neuralnet.cpp
    src/data_logger.cpp
)

add_executable(evolutionary_rl ${SOURCES})

if(APPLE)
    target_compile_options(evolutionary_rl PRIVATE -O3 -march=native)
elseif(UNIX)
    target_compile_options(evolutionary_rl PRIVATE -O3 -march=native)
elseif(MSVC)
    target_compile_options(evolutionary_rl PRIVATE /O2)
endif()
