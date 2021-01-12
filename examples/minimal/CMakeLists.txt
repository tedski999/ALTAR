
project(minimal)
add_executable(minimal minimal.c)
target_link_libraries(minimal ALTAR glfw z zip)

