#ifndef ALTAR_UTILS_ERROR_H
#define ALTAR_UTILS_ERROR_H

#define altar_assert(assertion, message, ...) if (!(assertion)) altar_error(message __VA_OPT__(,) __VA_ARGS__)

#ifdef ALTAR_DEBUG
#define altar_assert_debug(assertion, message, ...) altar_assert(assertion, message __VA_OPT__(,) __VA_ARGS__)
#else
#define altar_assert_debug(assertion, message, ...)
#endif

void altar_error(const char *const message, ...);
void altar_error_fatal(void);

#endif

