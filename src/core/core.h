#ifndef ALTAR_CORE_H
#define ALTAR_CORE_H

#define _XOPEN_SOURCE 500

#define ALTAR_NANOSEC_PER_SEC (1000000000)
#define ALTAR_NANOSEC_PER_TICK (ALTAR_NANOSEC_PER_SEC / 120)
#define ALTAR_MAX_NANOSEC_BEHIND (ALTAR_NANOSEC_PER_TICK * 60)

#ifndef ALTAR_PLATFORM
#if defined (_WIN32)
#define ALTAR_PLATFORM WINDOWS
#define ALTAR_PATH_DELIMITER '\\'
#elif defined (__linux__)
#define ALTAR_PLATFORM LINUX
#define ALTAR_PATH_DELIMITER '/'
#else
#error "ALTAR does not currently support your target operating system!"
#endif
#endif

#endif

