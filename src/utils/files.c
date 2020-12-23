#include "files.h"
#include "log.h"
#include "error.h"
#include "memory.h"
#include "../core/core.h"
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#if ALTAR_PLATFORM_WINDOWS
#include <windows.h>
#define mkdir(dir, mode) mkdir(dir)
#elif ALTAR_PLATFORM_LINUX
#include <sys/stat.h>
#include <unistd.h>
#endif

static void altar_utils_files_internal_writeToFile(const char *file_path, const char *data, const char *file_mode);
static void altar_utils_files_internal_ensureFileDirectoryExists(const char *file_path);

char *altar_utils_files_getDataDirectory() {
	return strdup("." ALTAR_PLATFORM_PATH_DELIMITER); // TODO: SHGetFolderPath or XDG Base directory
}

void altar_utils_files_overwrite(const char *const file_path, const char *const data) {
	altar_utils_files_internal_writeToFile(file_path, data, "w");
}

void altar_utils_files_append(const char *const file_path, const char *const data) {
	altar_utils_files_internal_writeToFile(file_path, data, "a");
}

void altar_utils_files_mkdir(const char *const directory) {
	mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

// FIXME: it doesn't look like compressing with zlib is working on Windows right now
void altar_utils_files_compress_gz(const char *const src_file, const char *const dst_file) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Compressing data at '%s' to '%s' with gzip...", src_file, dst_file);

	altar_utils_files_internal_ensureFileDirectoryExists(src_file);
	altar_utils_files_internal_ensureFileDirectoryExists(dst_file);

	FILE *src = fopen(src_file, "rb");
	gzFile dst = gzopen(dst_file, "wb");
	char buffer[128];
	int bytes_read = 0;
	while ((bytes_read = fread(buffer, 1, sizeof buffer, src)) > 0)
		gzwrite(dst, buffer, bytes_read);

	fclose(src);
	gzclose(dst);
}

static void altar_utils_files_internal_writeToFile(const char *file_path, const char *data, const char *file_mode) {
	altar_utils_files_internal_ensureFileDirectoryExists(file_path);

	FILE *file = fopen(file_path, file_mode);
	if (!file)
		altar_utils_error("Unable to open file '%s'!", file_path);

	fputs(data, file);
	fclose(file);
}

static void altar_utils_files_internal_ensureFileDirectoryExists(const char *file_path) {
	char *directory = strdup(file_path);
	strrchr(directory, ALTAR_PLATFORM_PATH_DELIMITER[0])[1] = '\0';
	altar_utils_files_mkdir(directory);
	altar_free(directory);
}

