# Set the desired name of the executable file that will be built
set(OUTPUT_EXENAME Main_exe)
# Set the name of the target in CMake
set(EXENAME Main)
# Create an executable target with the specified name and source file(s)
add_executable(${EXENAME} main.cpp)
# Link the target (EXENAME='Main') to any required libraries, in this case 'Utilities'
target_link_libraries(${EXENAME} Utilities)
# Include the header-only library 'indicators'
target_include_directories(${EXENAME} PRIVATE /Users/sebastianyde/Tools/indicators/include)
# Set the output name of the target to the specified value of 'OUTPUT_EXENAME'
set_target_properties(${EXENAME} PROPERTIES OUTPUT_NAME ${OUTPUT_EXENAME})


# Set the desired name of the executable file that will be built
set(OUTPUT_EXENAME_2 Extra_exe)
# Set the name of the target in CMake
set(EXENAME_2 Extra)
# Create an executable target with the specified name and source file(s)
add_executable(${EXENAME_2} extra.cpp)
# Link the target (EXENAME_2='Extra') to any required libraries, in this case 'Utilities'
target_link_libraries(${EXENAME_2} Utilities)
# Include the header-only library 'indicators'
target_include_directories(${EXENAME_2} PRIVATE /Users/sebastianyde/Tools/indicators/include)
# Set the output name of the target to the specified value of 'OUTPUT_EXENAME'
set_target_properties(${EXENAME_2} PROPERTIES OUTPUT_NAME ${OUTPUT_EXENAME_2})

