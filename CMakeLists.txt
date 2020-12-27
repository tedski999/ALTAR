
cmake_minimum_required(VERSION 3.13)

project(
	ALTAR
	DESCRIPTION "Another Less Than Average Raycaster"
	LANGUAGES C
)

# Optionals
option(ALTAR_INSTALL "Enable installing the library" ON)
option(ALTAR_BUILD_EXAMPLES "Build the ALTAR example programs" OFF)
set(ALTAR_WINDOW_VENDOR GLFW CACHE STRING "Choose which window vendor to use")
set_property(CACHE ALTAR_WINDOW_VENDOR PROPERTY STRINGS GLFW)

# Install the public header
if(ALTAR_INSTALL)
	install(
		DIRECTORY include/ALTAR
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include
		FILES_MATCHING PATTERN ALTAR.h)
endif()

# Subdirectories
add_subdirectory(src)
if(ALTAR_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

