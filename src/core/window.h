#ifndef ALTAR_CORE_WINDOW_H
#define ALTAR_CORE_WINDOW_H

#include "../core/core.h"

#if ALTAR_PLATFORM_GLFW
#include "../vendor/GLFW/window.h"
#define altar_window_create() altar_glfw_window_create()
#define altar_window_update(window) altar_glfw_window_update(window)
#define altar_window_draw(window) altar_glfw_window_draw(window)
#define altar_window_destroy(window) altar_glfw_window_destroy(window)
#endif

#endif

