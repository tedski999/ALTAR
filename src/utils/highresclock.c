#include "highresclock.h"
#include "log.h"
#include "memory.h"
#include "../core/core.h"
#ifdef ALTAR_PLATFORM_LINUX
#define _POSIX_C_SOURCE (200809L)
#include <time.h>
#elif ALTAR_PLATFORM_WINDOWS
#include <windows.h>
#endif

struct altar_highresclock {
#ifdef ALTAR_PLATFORM_LINUX
	struct timespec time;
#elif ALTAR_PLATFORM_WINDOWS
	LARGE_INTEGER time;
#endif
};

struct altar_highresclock *altar_utils_highresclock_create(void) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Creating new high resolution clock...");
	struct altar_highresclock *new_clock = altar_malloc(sizeof *new_clock);
	altar_utils_highresclock_resetTime(new_clock);
	return new_clock;
}

unsigned long altar_utils_highresclock_resetTime(struct altar_highresclock *clock) {
	unsigned long time = altar_utils_highresclock_timeInNanoseconds(clock);
#ifdef ALTAR_PLATFORM_LINUX
	clock_gettime(CLOCK_MONOTONIC, &clock->time);
#elif ALTAR_PLATFORM_WINDOWS
	QueryPerformanceCounter(&clock->time);
#endif
	return time;
}

double altar_utils_highresclock_timeInSeconds(struct altar_highresclock *clock) {
#ifdef ALTAR_PLATFORM_LINUX
	return (double) altar_utils_highresclock_timeInNanoseconds(clock) / ALTAR_NANOSEC_PER_SEC;
#elif ALTAR_PLATFORM_WINDOWS
	LARGE_INTEGER frequency, current_time;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&current_time);
	return (double) (current_time.QuadPart - clock->time.QuadPart) / frequency.QuadPart;
#endif
}

unsigned long altar_utils_highresclock_timeInNanoseconds(struct altar_highresclock *clock) {
#ifdef ALTAR_PLATFORM_LINUX
	struct timespec current_time;
	clock_gettime(CLOCK_MONOTONIC, &current_time);
	return ((current_time.tv_sec - clock->time.tv_sec) * ALTAR_NANOSEC_PER_SEC) + (current_time.tv_nsec - clock->time.tv_nsec);
#elif ALTAR_PLATFORM_WINDOWS
	return (unsigned long) altar_utils_highresclock_timeInSeconds(clock) * ALTAR_NANOSEC_PER_SEC;
#endif
}

void altar_utils_highresclock_destroy(struct altar_highresclock *clock) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Destroying high resolution clock...");
	altar_free(clock);
}

