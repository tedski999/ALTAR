#ifndef ALTAR_UTILS_LOG_H
#define ALTAR_UTILS_LOG_H

#include <stdarg.h>

enum altar_log_urgency {
	ALTAR_VERBOSE_LOG,
	ALTAR_INFO_LOG,
	ALTAR_NOTEWORTHY_LOG,
	ALTAR_WARNING_LOG,
	ALTAR_ERROR_LOG,
	altar_log_urgency_count
};

void altar_utils_log_init(void);
void altar_utils_log(const enum altar_log_urgency urgency, const char *const message, ...);
void altar_utils_log_variadic(const enum altar_log_urgency urgency, const char *const message, va_list args);
void altar_utils_log_cleanup(void);

#endif

