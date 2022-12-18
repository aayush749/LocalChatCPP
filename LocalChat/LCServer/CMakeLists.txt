cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# this is set to add Networking library to LCServer, since Networking is not built inside LCServer
cmake_policy(SET CMP0079 NEW)


project("LCServer")


#include dirs
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Networking/include)

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/core)

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/LCServer/include)

#sources
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/Message.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/TextMessage.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCServer/src/LCServer/LCServer.cpp)

list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCServer/LCServLauncher.cpp)

#libraries
list(APPEND LIBS Networking)

add_executable(LCServer ${SOURCES})

target_include_directories(LCServer PRIVATE ${INCLUDE_DIRS})
target_link_libraries(LCServer ${LIBS})

if(MSVC)
	target_compile_options(LCServer PUBLIC "/ZI")
	target_link_options(LCServer PUBLIC "/INCREMENTAL")
endif()