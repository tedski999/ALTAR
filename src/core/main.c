#include "core.h"
#include "../utils/log.h"
#include "../utils/highresclock.h"
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

// TODO: testing only
#include "../utils/files.h"
#include "../utils/memory.h"

extern void altar_entrypoint(void);
static void altar_core_internal_cleanup(void);
static void altar_core_internal_signalCallback(int signal);

static bool is_running = true;

int main(void) {
	atexit(altar_core_internal_cleanup);
	altar_utils_log_init();

	altar_utils_log(ALTAR_INFO_LOG, "Initializing game engine...");

	// TODO: these don't handle Windows close callbacks
	altar_utils_log(ALTAR_VERBOSE_LOG, "Registering system signal callbacks...");
	signal(SIGINT, altar_core_internal_signalCallback);
	signal(SIGTERM, altar_core_internal_signalCallback);
	signal(SIGABRT, altar_core_internal_signalCallback);
#ifdef ALTAR_PLATFORM_LINUX
	signal(SIGHUP, altar_core_internal_signalCallback);
	signal(SIGUSR1, altar_core_internal_signalCallback);
	signal(SIGUSR2, altar_core_internal_signalCallback);
	signal(SIGQUIT, altar_core_internal_signalCallback);
#endif

	// TODO: remove these tests
	char *program_directory = altar_utils_files_getProgramDirectory();
	altar_utils_log(ALTAR_NOTEWORTHY_LOG, "Program directory test: %s", program_directory);
	char *archive_file = altar_utils_files_concatPaths(program_directory, "data.wad");
	struct altar_archive *archive = altar_utils_files_openArchive(archive_file);
	altar_utils_log(ALTAR_NOTEWORTHY_LOG, "Archive read test: %s", altar_utils_files_readFromArchive(archive, "testData.txt"));
	altar_utils_files_closeArchive(archive);
	altar_free(program_directory);
	altar_free(archive_file);

	altar_utils_log(ALTAR_INFO_LOG, "Executing client entrypoint function...");
	altar_entrypoint();

	is_running = true;
	unsigned int accumulated_time = 0;
	struct altar_highresclock *clock = altar_utils_highresclock_create();
	altar_utils_log(ALTAR_INFO_LOG, "Initialization complete! Entering main game loop...");
	do {
		accumulated_time += altar_utils_highresclock_resetTime(clock);

		if (accumulated_time > ALTAR_MAX_NANOSEC_BEHIND) {
			altar_utils_log(ALTAR_WARNING_LOG, "Can't keep up! Skipping queued update cycles...");
			accumulated_time = ALTAR_NANOSEC_PER_TICK;
		}

		while (accumulated_time >= ALTAR_NANOSEC_PER_TICK) {
			accumulated_time -= ALTAR_NANOSEC_PER_TICK;
			// TODO: perform update tick
		}

		// TODO: draw game

	} while (is_running);

	altar_utils_highresclock_destroy(clock);
	altar_utils_log(ALTAR_INFO_LOG, "Exited main game loop.");
}

static void altar_core_internal_cleanup(void) {
	altar_utils_log(ALTAR_INFO_LOG, "Cleaning up...");
	altar_utils_log_cleanup();
}

static void altar_core_internal_signalCallback(int sig) {
	//altar_utils_log(ALTAR_NOTEWORTHY_LOG, "Received signal %i: %s", sig, strsignal(sig)); // FIXME: msvc version needed
	is_running = false;
}

