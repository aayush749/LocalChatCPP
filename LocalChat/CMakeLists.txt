# CMakeList.txt : CMake project for LocalChat, include source and define
# project specific logic here.
#

option(BUILD_SERVER "Builds only the LCServer" OFF)
option(BUILD_CLIENT "Builds only the LCClient" OFF)
option(BUILD_SERVER_AND_CLIENT "Builds both LCServer and LCClient" ON)

if(BUILD_SERVER)
	add_subdirectory("LCServer")
endif()

if(BUILD_CLIENT)
	add_subdirectory("LCClient")
endif()

if(BUILD_SERVER_AND_CLIENT)
	add_subdirectory("LCServer")
	add_subdirectory("LCClient")
endif()

add_subdirectory("vendor/crossguid")