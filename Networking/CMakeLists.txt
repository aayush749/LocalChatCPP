cmake_minimum_required (VERSION 3.1)

# this is set to include winsock2 library
cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Networking/include")

list(APPEND SOURCES ${INCLUDE_DIR}/Networking/Socket.h)
list(APPEND SOURCES ${INCLUDE_DIR}/Networking/ServerSocket.h)
list(APPEND SOURCES ${INCLUDE_DIR}/Networking/Networking.h)

list(APPEND SOURCES src/EntryPoint.cpp)
list(APPEND SOURCES src/Socket.cpp)
list(APPEND SOURCES src/ServerSocket.cpp)

if(WIN32)
    if (MSVC)
        find_library(WS2_32_LIBRARY ws2_32)
    elseif (MINGW)
        find_library(WS2_32_LIBRARY libws2_32.a) 
    endif()       
    list(APPEND LIBS ${WS2_32_LIBRARY})
endif()

add_library(Networking SHARED ${SOURCES})

target_include_directories(Networking PRIVATE ${INCLUDE_DIR})
target_link_libraries(Networking ${LIBS})