#include "error.h"
#include "log.h"
#include "memory.h"
#include "../core/core.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef ALTAR_DEBUG
#if ALTAR_PLATFORM == LINUX
#include <execinfo.h>
#elif ALTAR_PLATFORM == WINDOWS
#include <windows.h>
#include <dbghelp.h>
#endif
#define ALTAR_CALLSTACK_BUFFER_SIZE 128
#endif

#if ALTAR_PLATFORM == LINUX
#define ALTAR_NORETURN __attribute__((noreturn))
#define ALTAR_UNREACHABLE __builtin_unreachable();
#elif ALTAR_PLATFORM == WINDOWS
#define ALTAR_NORETURN __declspec(noreturn)
#define ALTAR_UNREACHABLE __assume(0)
#endif

ALTAR_NORETURN
void altar_utils_error(const char *const message, ...) {
	if (message) {
		va_list args, args_copy;
		va_start(args, message);
		va_copy(args_copy, args);
		int formatted_message_length = vsnprintf(NULL, 0, message, args_copy) + 1;
		char *formatted_message = altar_malloc(formatted_message_length);
		if (formatted_message) {
			vsprintf(formatted_message, message, args);
			altar_utils_log(ALTAR_ERROR_LOG, formatted_message);
		}
		va_end(args_copy);
		va_end(args);
	}

#ifdef ALTAR_DEBUG
	void *callstack[ALTAR_CALLSTACK_BUFFER_SIZE];

#if ALTAR_PLATFORM == LINUX
	int frame_count = backtrace(callstack, ALTAR_CALLSTACK_BUFFER_SIZE);
	char **strs = backtrace_symbols(callstack, frame_count);
	altar_utils_log(ALTAR_WARNING_LOG, "Callstack (%i frames):", frame_count);
	for (int i = 0; i < frame_count; i++)
		altar_utils_log(ALTAR_WARNING_LOG, "\t%s", strs[i]);
	altar_free(strs);
#elif ALTAR_PLATFORM == WINDOWS
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, 1);
	unsigned short frames = CaptureStackBackTrace(0, ALTAR_CALLSTACK_BUFFER_SIZE, callstack, NULL);
	SYMBOL_INFO *symbol = calloc(sizeof *symbol + 256, 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof *symbol;
	for(int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64) callstack[i], 0, symbol);
		altar_utils_log(ALTAR_WARNING_LOG, "\t%i: %s - 0x%0X", frames - i - 1, symbol->Name, symbol->Address);
	}
	altar_free(symbol);
#endif
#endif

	exit(1);
	ALTAR_UNREACHABLE;
}

ALTAR_NORETURN
void altar_utils_error_fatal(void) {
	exit(-1);
	ALTAR_UNREACHABLE;
}

