# Set the name of the test executable target
set(TESTNAME functions_test)

# Create an executable target with the specified name and source file
add_executable(${TESTNAME} functions_test.cpp)

# Find the Catch2 testing framework and set the search path to /opt/homebrew/Cellar/catch2/3.0.1/include
find_package(Catch2 REQUIRED PATHS /opt/homebrew/Cellar/catch2/3.1.1/include)

# Set the compiler options for the test target to include additional warnings
target_compile_options(${TESTNAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)

# Link the test target to the 'Utilities' library (name set in src/lib/CmakeLists.txt) and 'Catch2::Catch2WithMain' libraries
target_link_libraries(${TESTNAME} PRIVATE Utilities Catch2::Catch2WithMain)
