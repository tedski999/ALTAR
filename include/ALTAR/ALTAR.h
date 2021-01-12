#ifndef ALTAR_H
#define ALTAR_H

enum altar_log_urgency {
	ALTAR_VERBOSE_LOG,
	ALTAR_INFO_LOG,
	ALTAR_NOTEWORTHY_LOG,
	ALTAR_WARNING_LOG,
	ALTAR_ERROR_LOG,
	altar_log_urgency_count
};

// Helper macro for easily checking if a result is not false
#define altar_assert(assertion, message, ...) if (!(assertion)) altar_error(message __VA_OPT__(,) __VA_ARGS__)

// Log formatted message to stdout and the log file.
void altar_log(enum altar_log_urgency urgency, const char *const message, ...);

// Log and then exit prematurely due to an error. Exit code is 1.
void altar_error(const char *const message, ...);

#endif

