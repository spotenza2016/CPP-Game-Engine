cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(GameEngine)

set(SFML_DIR "D:/Downloads/SFML32/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

add_executable(GameEngine main.cpp Engine.cpp Engine.h Object.cpp Object.h QuadTree.cpp QuadTree.h)

target_link_libraries(GameEngine sfml-graphics sfml-audio)