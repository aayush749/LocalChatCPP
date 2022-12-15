# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.1)

# this is set to include winsock2 library
cmake_policy(SET CMP0079 NEW)

project ("LocalChat")

# conan packages include
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

# call conan setup macro
conan_basic_setup()

# Include sub-projects.
add_subdirectory ("LocalChat")
add_subdirectory ("Networking")

if(MSVC)
	target_compile_options(LocalChat PUBLIC "/ZI")
	target_link_options(LocalChat PUBLIC "/INCREMENTAL")
endif()

# Add the Networking library
list(APPEND LIBS Networking)

target_link_libraries(LocalChat ${LIBS})