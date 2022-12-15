# CMakeList.txt : CMake project for LocalChat, include source and define
# project specific logic here.
#

#include dirs
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Networking/include)

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/core)

#sources
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/Message.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LocalChat.cpp)


# Add source to this project's executable.
add_executable (LocalChat ${SOURCES})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LocalChat PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(LocalChat PRIVATE ${INCLUDE_DIRS})

# TODO: Add tests and install targets if needed.
