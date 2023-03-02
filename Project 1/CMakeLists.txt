cmake_minimum_required(VERSION 3.22)
project(Project_1  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp Include/Utilities.cpp)
target_include_directories(main PUBLIC Include)
