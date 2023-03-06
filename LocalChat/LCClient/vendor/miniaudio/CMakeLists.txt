project(miniaudio)

add_library(miniaudio STATIC ${CMAKE_CURRENT_LIST_DIR}/src/miniaudio.c)

target_include_directories(miniaudio PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)