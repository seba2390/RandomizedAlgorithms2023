# Use 'file()' and 'GLOB' to create a list of source files that match the pattern '*.cpp'
file(GLOB SOURCE_FILES *.cpp)

# Set the name of the library target
set(LIBNAME Utilities)

# Create a static library target with the specified name and source files
add_library(${LIBNAME} STATIC ${SOURCE_FILES})
