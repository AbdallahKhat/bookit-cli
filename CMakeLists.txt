cmake_minimum_required(VERSION 3.16)

project(bookit VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(bookit)

target_sources(bookit PRIVATE
    src/Command.cpp
    src/CommandInfo.cpp
    src/CommandParser.cpp
    src/Core.cpp
    src/LibraryManager.cpp
    src/Workspaces.cpp
    src/main.cpp
)

target_include_directories(bookit 
    PRIVATE include 
    SYSTEM PRIVATE third_party)

target_compile_options(
    bookit
    PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow
    -pedantic-errors)

set_target_properties(bookit PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
