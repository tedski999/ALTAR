
# TODO: CMake for Visual Studio needs a bit of love:
#        - Most of the compile options are wrong for MSVC
#        - zlib isn't being linked properly, nevermind statically
#        - I didn't even attempt an 'install' of the library
#        - Dbghelp.lib isn't linked when needed
#        - Bunch of subprojects created that don't need to be: ZERO_CHECK, BUILD_ALL, etc

cmake_minimum_required(VERSION 3.10)
project(ALTAR)

file(GLOB_RECURSE src_files src/*.c)
add_library(ALTAR STATIC ${src_files})

set(CMAKE_C_STANDARD 89)
set(CMAKE_C_STANDARD_REQUIRED True)
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

target_include_directories(ALTAR PUBLIC src)
target_compile_options(ALTAR PUBLIC "-Wall" "-Wextra")
target_compile_options(ALTAR PUBLIC "$<$<CONFIG:DEBUG>:-Og;-g3>")
target_compile_options(ALTAR PUBLIC "$<$<CONFIG:RELEASE>:-O3>")
target_compile_definitions(ALTAR PRIVATE "$<$<CONFIG:DEBUG>:ALTAR_DEBUG>")

set_target_properties(
	ALTAR
	PROPERTIES
	PUBLIC_HEADER "include/ALTAR.h"
)

install(
	TARGETS ALTAR
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include/ALTAR
)

