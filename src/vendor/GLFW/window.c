#include "window.h"
#include "../../utils/log.h"
#include "../../utils/error.h"
#include "../../utils/memory.h"
#include <GLFW/glfw3.h>

struct altar_window {
	GLFWwindow *window;
};

static unsigned open_windows_count = 0;

struct altar_window *altar_glfw_window_create(void) {
	if (open_windows_count++ == 0) {
		altar_log(ALTAR_VERBOSE_LOG, "Initializing GLFW...");
		altar_assert(glfwInit(), "Unable to initialze GLFW!");
	}

	altar_log(ALTAR_INFO_LOG, "Opening new GLFW window...");
	struct altar_window *new_window = altar_malloc(sizeof *new_window);
	new_window->window = glfwCreateWindow(640, 480, "hello world", NULL, NULL);
	altar_assert(new_window->window, "Unable to create GLFW window!");

	glfwMakeContextCurrent(new_window->window);
	glfwSwapInterval(1);

	// TODO: setup window event callbacks

	return new_window;
}

void altar_glfw_window_update(struct altar_window *window) {
	glfwPollEvents();
}

void altar_glfw_window_draw(struct altar_window *window) {
	//glfwMakeContextCurrent(window->window); // NOTE: unsure if this will be needed with multi-windowed programs
	glfwSwapBuffers(window->window);
}

void altar_glfw_window_destroy(struct altar_window *window) {
	altar_log(ALTAR_INFO_LOG, "Closing GLFW window...");
	glfwDestroyWindow(window->window);
	altar_free(window);
	if (--open_windows_count == 0) {
		altar_log(ALTAR_VERBOSE_LOG, "Terminating GLFW...");
		glfwTerminate();
	}
}

