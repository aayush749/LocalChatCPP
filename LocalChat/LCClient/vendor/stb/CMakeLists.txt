cmake_minimum_required (VERSION 3.1)

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.cpp)

add_library(stb_image STATIC ${SOURCES})

target_include_directories(stb_image PRIVATE ${INCLUDE_DIRS})