cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# this is set to add Networking library to LCClient, since Networking is not built inside LCClient
cmake_policy(SET CMP0079 NEW)


project("LCClient")

add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/inipp)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/Glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/GLFW)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/stb)

#include dirs
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Logger/include)
list(APPEND INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/Glad/include)
list(APPEND INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/inipp)
list(APPEND INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/imgui)
list(APPEND INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/imgui/backends)
list(APPEND INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/stb/include)

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Networking/include)

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/core)
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/include)
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/LocalChat/vendor/crossguid/include)

#sources

#imgui opengl3 source
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/backends/imgui_impl_opengl3.cpp)

#imgui sources
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/imgui.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/imgui_demo.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/imgui_draw.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/imgui_tables.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/imgui_widgets.cpp)

#imgui glfw sources
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/vendor/imgui/backends/imgui_impl_glfw.cpp)


list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/Message.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/TextMessage.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/core/Message/ControlMessage.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/src/SerializationUtils.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/src/Conversion.cpp)


list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/src/LCClient/LCClient.cpp)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/LocalChat/LCClient/LCClientMain.cpp)

#libraries
list(APPEND LIBS Networking)
list(APPEND LIBS inipp::inipp)
list(APPEND LIBS crossguid)
list(APPEND LIBS Glad)
list(APPEND LIBS glfw)
list(APPEND LIBS stb_image)

add_executable(LCClient ${SOURCES})

target_include_directories(LCClient PRIVATE ${INCLUDE_DIRS})
target_link_libraries(LCClient ${LIBS})

if(MSVC)
	target_compile_options(LCClient PUBLIC "/ZI")
	target_link_options(LCClient PUBLIC "/INCREMENTAL")
endif()