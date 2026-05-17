cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(speedshifter VERSION 1.0.0 LANGUAGES CXX)

# Find the Qt Quick library
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_policy(SET QTP0004 NEW)
qt_standard_project_setup()

add_subdirectory(src)
