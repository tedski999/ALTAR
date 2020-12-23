#include "files.h"
#include "log.h"
#include "error.h"
#include "memory.h"
#include "../core/core.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#if ALTAR_PLATFORM_LINUX
#include <sys/stat.h>
#include <unistd.h>
#elif ALTAR_PLATFORM_WINDOWS
#include <windows.h>
#define mkdir(dir, mode) mkdir(dir)
#endif

static void altar_utils_files_internal_writeToFile(const char *const file_path, const char *const data, const char *const file_mode);
static void altar_utils_files_internal_mkdirForFile(const char *const file_path);

char *altar_utils_files_getDataDirectory(void) {
	const char *envvar = NULL;
	char *known_directory = NULL, *data_directory = NULL;

#if ALTAR_PLATFORM_LINUX
	envvar = getenv("XDG_DATA_HOME");
	known_directory = (envvar) ? strdup(envvar) : altar_utils_files_concatPaths(getenv("HOME"), ".local/share");
#elif ALTAR_PLATFORM_WINDOWS
	envvar = getenv("LOCALAPPDATA");
	known_directory = (envvar) ? _strdup(envvar) : altar_utils_files_concatPaths(getenv("PROGRAMDATA"), "ALTAR");
#endif

	// FIXME: append client-defined "<app name>" instead
	if (known_directory)
		data_directory = altar_utils_files_concatPaths(known_directory, "ALTAR");
	else
		altar_utils_log(ALTAR_WARNING_LOG, "Unable to locate user data directory - file logging disabled!");

	free(known_directory);
	return data_directory;
}

char *altar_utils_files_concatPaths(const char *const a, const char *const b) {
	if (!a || !b)
		return NULL;
	char *result = altar_malloc(strlen(a) + strlen(b) + 2);
	sprintf(result, "%s%c%s", a, ALTAR_PLATFORM_PATH_DELIMITER, b);
	return result;
}

void altar_utils_files_overwrite(const char *const file_path, const char *const data) {
	altar_utils_files_internal_writeToFile(file_path, data, "w");
}

void altar_utils_files_append(const char *const file_path, const char *const data) {
	altar_utils_files_internal_writeToFile(file_path, data, "a");
}

void altar_utils_files_compress_gz(const char *const src_file, const char *const dst_file) {
	if (!src_file || !dst_file)
		return;

	altar_utils_log(ALTAR_VERBOSE_LOG, "Compressing data at '%s' to '%s' with gzip...", src_file, dst_file);

	altar_utils_files_internal_mkdirForFile(dst_file);
	FILE *src = fopen(src_file, "rb");
	gzFile dst = gzopen(dst_file, "wb");
	char buffer[128];
	int bytes_read = 0;
	while ((bytes_read = fread(buffer, 1, sizeof buffer, src)) > 0)
		gzwrite(dst, buffer, bytes_read);

	fclose(src);
	gzclose(dst);
}

static void altar_utils_files_internal_writeToFile(const char *const file_path, const char *const data, const char *const file_mode) {
	if (!file_path || !data)
		return;

	FILE *file = fopen(file_path, file_mode);
	if (!file)
		altar_utils_files_internal_mkdirForFile(file_path);
	file = fopen(file_path, file_mode);
	if (!file)
		altar_utils_error("Unable to open file '%s'!", file_path);

	fputs(data, file);
	fclose(file);
}

// calls mkdir on every directory in path
static void altar_utils_files_internal_mkdirForFile(const char *const file_path) {
	char *directory = strdup(file_path);
	char *ptr = directory;
	while (*++ptr) {
		if (*ptr == ALTAR_PLATFORM_PATH_DELIMITER) {
			*ptr = '\0';
			mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			*ptr = ALTAR_PLATFORM_PATH_DELIMITER;
		}
	}

	altar_free(directory);
}

