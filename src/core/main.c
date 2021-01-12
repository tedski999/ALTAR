#include "core.h"
#include "window.h"
#include "../utils/log.h"
#include "../utils/highresclock.h"
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

extern void altar_main(void);
static void altar_core_internal_cleanup(void);
static void altar_core_internal_signalCallback(int signal);

static bool is_running;
static struct altar_highresclock *clock;
static struct altar_window *window;

int main(void) {
	atexit(altar_core_internal_cleanup);
	altar_log_init();

	altar_log(ALTAR_INFO_LOG, "Initializing ALTAR...");

	altar_log(ALTAR_VERBOSE_LOG, "Registering system signal callbacks...");
	signal(SIGINT, altar_core_internal_signalCallback);
	signal(SIGTERM, altar_core_internal_signalCallback);
	signal(SIGABRT, altar_core_internal_signalCallback);
#if ALTAR_PLATFORM_LINUX
	signal(SIGHUP, altar_core_internal_signalCallback);
	signal(SIGUSR1, altar_core_internal_signalCallback);
	signal(SIGUSR2, altar_core_internal_signalCallback);
	signal(SIGQUIT, altar_core_internal_signalCallback);
#endif

	window = altar_window_create();

	altar_log(ALTAR_INFO_LOG, "Executing client entrypoint function...");
	altar_main();

	is_running = true;
	unsigned int accumulated_time = 0;
	clock = altar_highresclock_create();
	altar_log(ALTAR_INFO_LOG, "Initialization complete! Entering main game loop...");
	while (is_running) {
		accumulated_time += altar_highresclock_resetTime(clock);

		if (accumulated_time > ALTAR_MAX_NANOSEC_BEHIND) {
			altar_log(ALTAR_WARNING_LOG, "Can't keep up! Skipping queued update cycles...");
			accumulated_time = ALTAR_NANOSEC_PER_TICK;
		}

		while (accumulated_time >= ALTAR_NANOSEC_PER_TICK) {
			accumulated_time -= ALTAR_NANOSEC_PER_TICK;
			altar_window_update(window);
		}

		altar_window_draw(window);

	}

	altar_log(ALTAR_INFO_LOG, "Exited main game loop.");
}

static void altar_core_internal_cleanup(void) {
	altar_log(ALTAR_INFO_LOG, "Cleaning up...");
	if (clock)
		altar_highresclock_destroy(clock);
	if (window)
		altar_window_destroy(window);
	altar_log_cleanup();
}

static void altar_core_internal_signalCallback(int sig) {
	altar_log(ALTAR_NOTEWORTHY_LOG, "Received signal #%i", sig);
	is_running = false;
}

