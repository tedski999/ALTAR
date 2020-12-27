#ifndef ALTAR_CORE_WINDOW_H
#define ALTAR_CORE_WINDOW_H

#if ALTAR_WINDOW_VENDOR == GLFW
#include "../vendor/window/GLFW/window.h"
#define altar_window_create() altar_glfwWindow_create()
#define altar_window_update(window) altar_glfwWindow_update(window)
#define altar_window_draw(window) altar_glfwWindow_draw(window)
#define altar_window_destroy(window) altar_glfwWindow_destroy(window)
#endif

#endif

