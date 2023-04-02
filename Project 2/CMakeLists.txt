# Set the minimum version of CMake required to build the project
cmake_minimum_required(VERSION 3.22)

# Set the name of the project and the programming language used
project(Project_2  LANGUAGES CXX)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Finding Boost
set(MY_BOOST_PATH "/opt/homebrew/Cellar/boost/1.81.0_1")
find_package(Boost REQUIRED PATHS ${MY_BOOST_PATH})
if(Boost_FOUND)
    message("===== Boost found! ======")
    if(DEFINED Boost_VERSION_STRING)
        message("Boost_VERSION_STRING is defined: ${Boost_VERSION_STRING}")
    else()
        message("Boost_VERSION_STRING is not defined!!")
    endif()
    if(DEFINED Boost_INCLUDE_DIRS)
        message("Boost_INCLUDE_DIRS is defined: ${Boost_INCLUDE_DIRS}")
    else()
        message("Boost_INCLUDE_DIRS is not defined!!")
        message("Setting Boost_INCLUDE_DIRS to:")
        set(MY_BOOST_INCLUDE_DIR "${MY_BOOST_PATH}/include")
        message(${MY_BOOST_INCLUDE_DIR})
        set(${Boost_INCLUDE_DIRS} ${MY_BOOST_INCLUDE_DIR})
    endif()
    if(DEFINED Boost_LIBRARY_DIRS)
        message("Boost_LIBRARY_DIRS is defined: ${Boost_LIBRARY_DIRS}")
    else()
        message("Boost_LIBRARY_DIRS is not defined!!")
        message("Setting Boost_LIBRARY_DIRS to:")
        set(MY_BOOST_LIBRARY_DIR "${MY_BOOST_PATH}/lib")
        message(${MY_BOOST_LIBRARY_DIR})
        set(${Boost_LIBRARY_DIRS} ${MY_BOOST_LIBRARY_DIR})
    endif()
endif()
include_directories(${Boost_INCLUDE_DIRS})


# Add the 'include' directory to the list of directories to search for header files
include_directories(include)

# Add the 'src' subdirectory, which contains the source code for the project
add_subdirectory(src)

# Add the 'test' subdirectory, which contains the unit tests for the project
add_subdirectory(test)
