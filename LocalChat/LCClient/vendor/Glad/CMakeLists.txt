cmake_minimum_required (VERSION 3.1)

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c)

find_library(GL_LIB opengl32.lib)

add_library(Glad STATIC ${SOURCES})

target_include_directories(Glad PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Glad PRIVATE ${GL_LIB})