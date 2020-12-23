#ifndef ALTAR_UTILS_ERROR_H
#define ALTAR_UTILS_ERROR_H

void altar_utils_error(const char *const message, ...);

// NOTE: Only call if something fundamental breaks, like memory or output.
// Specifically, if what's broken prevents the above call from working.
void altar_utils_error_fatal(void);

#endif

