# CMakeList.txt : CMake project for LocalChat, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (LocalChat "LocalChat.cpp" "LocalChat.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LocalChat PROPERTY CXX_STANDARD 20)
endif()


#include dirs
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Networking/include)

target_include_directories(LocalChat PRIVATE ${INCLUDE_DIRS})

# TODO: Add tests and install targets if needed.
