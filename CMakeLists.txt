# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.1)

# this is set to add Networking library to LocalChat, since Networking is not built inside LocalChat
cmake_policy(SET CMP0079 NEW)

# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# conan packages include
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

# call conan setup macro
conan_basic_setup()

# Include sub-projects.
add_subdirectory ("LocalChat")
add_subdirectory ("Networking")