cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# this is set to add Networking library to LCClient, since Networking is not built inside LCClient
cmake_policy(SET CMP0079 NEW)


project("LCClient")

#include dirs
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Networking/include)

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/core)
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/include)

#sources
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/Message.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/TextMessage.cpp)


list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/src/LCClient/LCClient.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/LCClientMain.cpp)

#libraries
list(APPEND LIBS Networking)

add_executable(LCClient ${SOURCES})

target_include_directories(LCClient PRIVATE ${INCLUDE_DIRS})
target_link_libraries(LCClient ${LIBS})

if(MSVC)
	target_compile_options(LCClient PUBLIC "/ZI")
	target_link_options(LCClient PUBLIC "/INCREMENTAL")
endif()