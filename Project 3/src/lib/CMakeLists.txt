# Use 'file()' and 'GLOB' to create a list of source files that match the pattern '*.cpp'
file(GLOB SOURCE_FILES *.cpp)

# Set the name of the library target
set(LIBNAME Utilities)

# Create a static library target with the specified name and source files
add_library(${LIBNAME} STATIC ${SOURCE_FILES} ../../include/lib/Utilities.hpp)


# Include the library boost
target_include_directories(${LIBNAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${LIBNAME}  ${Boost_LIBRARIES})

# Include the Eigen library
target_link_libraries(${LIBNAME} Eigen3::Eigen)