cmake_minimum_required(VERSION 3.22)
project(Project_1  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Eigen3 CONFIG REQUIRED)



add_executable(main main.cpp Include/Utilities.cpp)
target_include_directories(main PUBLIC Include)

target_link_libraries(main Eigen3::Eigen)