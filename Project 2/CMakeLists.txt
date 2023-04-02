# Set the minimum version of CMake required to build the project
cmake_minimum_required(VERSION 3.22)

# Set the name of the project and the programming language used
project(Project_2  LANGUAGES CXX)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the 'include' directory to the list of directories to search for header files
include_directories(include)

# Add the 'src' subdirectory, which contains the source code for the project
add_subdirectory(src)

# Add the 'test' subdirectory, which contains the unit tests for the project
add_subdirectory(test)
