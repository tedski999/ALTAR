#include "core.h"
#include "window.h"
#include "../utils/log.h"
#include "../utils/highresclock.h"
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

extern void altar_entrypoint(void);
static void altar_core_internal_cleanup(void);
static void altar_core_internal_signalCallback(int signal);

static bool is_running = true;
static struct altar_highresclock *clock;
static struct altar_window *window;

int main(void) {
	atexit(altar_core_internal_cleanup);
	altar_utils_log_init();

	altar_utils_log(ALTAR_INFO_LOG, "Initializing game engine...");

	altar_utils_log(ALTAR_VERBOSE_LOG, "Registering system signal callbacks...");
	signal(SIGINT, altar_core_internal_signalCallback);
	signal(SIGTERM, altar_core_internal_signalCallback);
	signal(SIGABRT, altar_core_internal_signalCallback);
#if ALTAR_PLATFORM == LINUX
	signal(SIGHUP, altar_core_internal_signalCallback);
	signal(SIGUSR1, altar_core_internal_signalCallback);
	signal(SIGUSR2, altar_core_internal_signalCallback);
	signal(SIGQUIT, altar_core_internal_signalCallback);
#endif

	window = altar_window_create();

	altar_utils_log(ALTAR_INFO_LOG, "Executing client entrypoint function...");
	altar_entrypoint();

	is_running = true;
	unsigned int accumulated_time = 0;
	clock = altar_utils_highresclock_create();
	altar_utils_log(ALTAR_INFO_LOG, "Initialization complete! Entering main game loop...");
	while (is_running) {
		accumulated_time += altar_utils_highresclock_resetTime(clock);

		if (accumulated_time > ALTAR_MAX_NANOSEC_BEHIND) {
			altar_utils_log(ALTAR_WARNING_LOG, "Can't keep up! Skipping queued update cycles...");
			accumulated_time = ALTAR_NANOSEC_PER_TICK;
		}

		while (accumulated_time >= ALTAR_NANOSEC_PER_TICK) {
			accumulated_time -= ALTAR_NANOSEC_PER_TICK;
			altar_window_update(window);
		}

		altar_window_draw(window);

	}

	altar_utils_log(ALTAR_INFO_LOG, "Exited main game loop.");
}

static void altar_core_internal_cleanup(void) {
	altar_utils_log(ALTAR_INFO_LOG, "Cleaning up...");
	altar_utils_highresclock_destroy(clock);
	altar_window_destroy(window);
	altar_utils_log_cleanup();
}

static void altar_core_internal_signalCallback(int sig) {
	//altar_utils_log(ALTAR_NOTEWORTHY_LOG, "Received signal %i: %s", sig, strsignal(sig)); // FIXME: msvc version needed
	is_running = false;
}

