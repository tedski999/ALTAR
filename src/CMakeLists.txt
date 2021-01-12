
add_library(ALTAR STATIC
	core/core.h
	core/main.c
	core/window.h
	utils/error.c utils/error.h
	utils/files.c utils/files.h
	utils/highresclock.c utils/highresclock.h
	utils/log.c utils/log.h
	utils/memory.c utils/memory.h
)

find_package(libzip CONFIG REQUIRED)
find_package(ZLIB MODULE REQUIRED)
target_link_libraries(ALTAR PRIVATE zip z)

# Enable relevant vendors
if(ALTAR_WINDOW_VENDOR STREQUAL "GLFW")
	find_package(glfw3 CONFIG REQUIRED)
	target_link_libraries(ALTAR PRIVATE glfw)
	target_sources(ALTAR PRIVATE vendor/GLFW/window.c vendor/GLFW/window.h)
	target_compile_definitions(ALTAR PRIVATE ALTAR_WINDOW_VENDOR=GLFW)
endif()

# Enable debuging features if requested
target_compile_definitions(ALTAR PRIVATE $<$<CONFIG:DEBUG>:ALTAR_DEBUG>)

# Enable relevant warnings
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
	if("${CMAKE_C_SIMULATE_ID}" STREQUAL "MSVC")
		target_compile_options(ALTAR PRIVATE "/clang:-Wall")
	else()
		target_compile_options(ALTAR PRIVATE "-Wall")
	endif()
elseif(MSVC)
	target_compile_options(ALTAR PRIVATE "/W3")
endif()
if(MSVC)
	target_compile_definitions(ALTAR PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Install library
if(ALTAR_INSTALL)
	install(TARGETS ALTAR
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	)
endif()

