#include "log.h"
#include "memory.h"
#include "files.h"
#include "highresclock.h"
#include "../core/core.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define ALTAR_LOG_DIRECTORY ("logs" ALTAR_PLATFORM_PATH_DELIMITER)
#define ALTAR_LOG_FILENAME ("latest.txt")
#define ALTAR_LOG_FORMAT ("%.4f %s[%s] %s%s\n")

static bool safe_to_log = false;
static struct altar_highresclock *init_clock;
static struct tm *local_date;
static char *log_file;
static char *compressed_file;
static const char *ALTAR_LOG_URGENCY_LABELS[altar_log_urgency_count] = { "VERB", "INFO", "NOTE", "WARN", "ERRR" };
static const char *ALTAR_LOG_URGENCY_COLORS[altar_log_urgency_count + 1] = { "\033[2;37m", "\033[0;37m", "\033[1;32m", "\033[1;33m", "\033[7;31m", "\033[0m" };

// sets init_time, generates paths for the logging file, prints date and time
void altar_utils_log_init(void) {
	init_clock = altar_utils_highresclock_create();
	time_t now = time(NULL);
	local_date = localtime(&now);

	// get the path of the directory to store logs
	char *data_directory = altar_utils_files_getDataDirectory();
	unsigned log_directory_length = strlen(data_directory) + strlen(ALTAR_LOG_DIRECTORY);
	char *log_directory = altar_realloc(data_directory, log_directory_length + 1);
	strcat(log_directory, ALTAR_LOG_DIRECTORY);

	// create the path to the log file
	log_file = altar_malloc(log_directory_length + strlen(ALTAR_LOG_FILENAME) + 1);
	strcpy(log_file, log_directory);
	strcat(log_file, ALTAR_LOG_FILENAME);

	// create the path to the compressed log file
	unsigned compressed_filename_length = 32;
	char *compressed_filename = altar_malloc(compressed_filename_length);
	while (!strftime(compressed_filename, compressed_filename_length, "%FT%T%Z.gz", local_date))
		compressed_filename = altar_realloc(compressed_filename, compressed_filename_length *= 2);
	compressed_file = altar_malloc(log_directory_length + strlen(compressed_filename) + 1);
	strcpy(compressed_file, log_directory);
	strcat(compressed_file, compressed_filename);
	altar_free(compressed_filename);

	// log the time and date
	unsigned time_string_length = 32;
	char *time_string = altar_malloc(time_string_length);
	while (!strftime(time_string, time_string_length, "%F %T %Z\n", local_date))
		time_string = altar_realloc(time_string, time_string_length *= 2);
	printf("%s", time_string);
	altar_utils_files_overwrite(log_file, time_string);
	altar_free(time_string);

	safe_to_log = true;
}

void altar_utils_log(enum altar_log_urgency urgency, const char *message, ...) {
	va_list args;
	va_start(args, message);
	altar_utils_log_variadic(urgency, message, args);
	va_end(args);
}

void altar_utils_log_variadic(enum altar_log_urgency urgency, const char *message, va_list args) {
#ifndef ALTAR_DEBUG
	if (urgency == ALTAR_VERBOSE_LOG)
		return;
#endif

	if (!safe_to_log)
		return;

	double time_since_init = altar_utils_highresclock_timeInSeconds(init_clock);

	// format message with args
	va_list args_copy;
	va_copy(args_copy, args);
	int formatted_message_length = vsnprintf(NULL, 0, message, args_copy) + 1;
	char *formatted_message = altar_malloc(formatted_message_length);
	vsprintf(formatted_message, message, args);
	va_end(args_copy);

	// format log with colors and labels
	int log_string_length = snprintf(
		NULL, 0, ALTAR_LOG_FORMAT,
		time_since_init, ALTAR_LOG_URGENCY_COLORS[urgency],
		ALTAR_LOG_URGENCY_LABELS[urgency], formatted_message,
		ALTAR_LOG_URGENCY_COLORS[altar_log_urgency_count]) + 1;
	char *log_string = altar_malloc(log_string_length);
	sprintf(
		log_string, ALTAR_LOG_FORMAT,
		time_since_init, ALTAR_LOG_URGENCY_COLORS[urgency],
		ALTAR_LOG_URGENCY_LABELS[urgency], formatted_message,
		ALTAR_LOG_URGENCY_COLORS[altar_log_urgency_count]);

	printf("%s", log_string);
	altar_utils_files_append(log_file, log_string);

	altar_free(formatted_message);
	altar_free(log_string);
}

void altar_utils_log_cleanup(void) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Ending logging session...");
	safe_to_log = false;
	altar_utils_files_compress_gz(log_file, compressed_file);
	altar_free(compressed_file);
	altar_free(log_file);
	altar_utils_highresclock_destroy(init_clock);
}

