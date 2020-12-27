#ifndef ALTAR_VENDOR_GLFW_WINDOW_H
#define ALTAR_VENDOR_GLFW_WINDOW_H

struct altar_window;

struct altar_window *altar_glfwWindow_create(void);
void altar_glfwWindow_update(struct altar_window *window);
void altar_glfwWindow_draw(struct altar_window *window);
void altar_glfwWindow_destroy(struct altar_window *window);

#endif

