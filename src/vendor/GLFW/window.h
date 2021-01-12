#ifndef ALTAR_VENDOR_GLFW_WINDOW_H
#define ALTAR_VENDOR_GLFW_WINDOW_H

struct altar_window;

struct altar_window *altar_glfw_window_create(void);
void altar_glfw_window_update(struct altar_window *window);
void altar_glfw_window_draw(struct altar_window *window);
void altar_glfw_window_destroy(struct altar_window *window);

#endif

