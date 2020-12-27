#include "window.h"
#include "../../../utils/log.h"
#include "../../../utils/error.h"
#include "../../../utils/memory.h"
#include <GLFW/glfw3.h>

struct altar_window {
	GLFWwindow *window;
};

static unsigned open_windows_count = 0;

struct altar_window *altar_glfwWindow_create(void) {
	if (open_windows_count++ == 0) {
		altar_utils_log(ALTAR_VERBOSE_LOG, "Initializing GLFW...");
		if (!glfwInit())
			altar_utils_error("Unable to initialze GLFW!");
	}

	altar_utils_log(ALTAR_INFO_LOG, "Opening new GLFW window...");
	struct altar_window *new_window = altar_malloc(sizeof *new_window);
	new_window->window = glfwCreateWindow(640, 480, "hello world", NULL, NULL);
	if (!new_window->window)
		altar_utils_error("Unable to create GLFW window!");

	glfwMakeContextCurrent(new_window->window);
	glfwSwapInterval(1);

	// TODO: setup window event callbacks

	return new_window;
}

void altar_glfwWindow_update(struct altar_window *window) {
	glfwPollEvents();
}

void altar_glfwWindow_draw(struct altar_window *window) {
	//glfwMakeContextCurrent(window->window); // NOTE: unsure if this will be needed with multi-windowed programs
	glfwSwapBuffers(window->window);
}

void altar_glfwWindow_destroy(struct altar_window *window) {
	altar_utils_log(ALTAR_INFO_LOG, "Closing GLFW window...");
	glfwDestroyWindow(window->window);
	altar_free(window);
	if (--open_windows_count == 0) {
		altar_utils_log(ALTAR_VERBOSE_LOG, "Terminating GLFW...");
		glfwTerminate();
	}
}

