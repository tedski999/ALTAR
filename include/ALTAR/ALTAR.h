#ifndef ALTAR_H
#define ALTAR_H

// FIXME: first defined in utils/log.h
enum altar_log_urgency {
	ALTAR_VERBOSE_LOG,
	ALTAR_INFO_LOG,
	ALTAR_NOTEWORTHY_LOG,
	ALTAR_WARNING_LOG,
	ALTAR_ERROR_LOG,
	altar_log_urgency_count
};

// Log formatted message to stdout and the log file.
void altar_utils_log(enum altar_log_urgency urgency, const char *const message, ...);

// Log and then exit prematurely due to an error.
// Exit code is 1.
void altar_utils_error(const char *const message, ...);

// Similar to altar_error, but does not attempt to log or cleanup at all
// Should only be used if logging or memory have failed in some way.
// Exit code is -1.
void altar_utils_error_fatel(void);

#endif

