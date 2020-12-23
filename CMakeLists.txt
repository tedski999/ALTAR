
cmake_minimum_required(VERSION 3.10)
project(ALTAR)

file(GLOB_RECURSE src_files src/*.c)
add_library(ALTAR STATIC ${src_files})

set(CMAKE_C_STANDARD 89)
set(CMAKE_C_STANDARD_REQUIRED True)
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

target_include_directories(ALTAR PUBLIC src)
target_compile_definitions(ALTAR PRIVATE $<$<CONFIG:DEBUG>:ALTAR_DEBUG>)

set_target_properties(
	ALTAR
	PROPERTIES
	PUBLIC_HEADER include/ALTAR.h
)

install(
	TARGETS ALTAR
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include/ALTAR
)

