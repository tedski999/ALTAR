#include "files.h"
#include "log.h"
#include "error.h"
#include "memory.h"
#include "../core/core.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <zip.h>
#if ALTAR_PLATFORM_LINUX
#include <sys/stat.h>
#include <unistd.h>
#elif ALTAR_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if ALTAR_PLATFORM_LINUX
#define locate_binary(buffer, buffer_size) readlink("/proc/self/exe", buffer, buffer_size)
#elif ALTAR_PLATFORM_WINDOWS
#define mkdir(dir, mode) mkdir(dir)
#define locate_binary(buffer, buffer_size) GetModuleFileNameA(NULL, buffer, buffer_size)
#endif

struct altar_archive {
	struct zip *zip;
};

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

char *altar_utils_files_getProgramDirectory(void) {

	// fit path to binary in path_buffer
	int path_buffer_used, path_buffer_size = 64;
	char *path_buffer = altar_malloc(path_buffer_size);
	while ((path_buffer_used = locate_binary(path_buffer, path_buffer_size - 1)) >= path_buffer_size - 2)
		path_buffer = altar_realloc(path_buffer, path_buffer_size *= 2);
	if (path_buffer_used <= 0)
		altar_utils_error("Unable to locate program directory!");

	// truncate binary filename from path
	path_buffer[path_buffer_used] = '\0';
	*strrchr(path_buffer, ALTAR_PLATFORM_PATH_DELIMITER) = '\0';
	return path_buffer;
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

struct altar_archive *altar_utils_files_openArchive(const char *const file_path) {
	altar_utils_log(ALTAR_INFO_LOG, "Opening data archive at '%s'...", file_path);
	struct altar_archive *archive = altar_malloc(sizeof *archive);

	int err = 0;
	archive->zip = zip_open(file_path, 0, &err);
	if (err) {
		altar_free(archive);
		struct zip_error error;
		zip_error_init_with_code(&error, err);
		altar_utils_error("Error opening archive: %s", zip_error_strerror(&error));
	}

	return archive;
}

struct altar_archive_data altar_utils_files_readFromArchive(struct altar_archive *archive, const char *const file_path) {
	altar_utils_log(ALTAR_INFO_LOG, "Reading '%s' from data archive...", file_path);

	altar_utils_log(ALTAR_VERBOSE_LOG, "Querying zip data...");
	struct zip_stat stat;
	zip_stat_init(&stat);
	if (zip_stat(archive->zip, file_path, 0, &stat))
		altar_utils_error("Error querying zip data: %s", zip_strerror(archive->zip));
	char *data = altar_malloc(stat.size + 1);

	altar_utils_log(ALTAR_VERBOSE_LOG, "Opening zip data...");
	struct zip_file *file = zip_fopen(archive->zip, file_path, 0);
	if (!file)
		altar_utils_error("Error opening zip data: %s", zip_strerror(archive->zip));

	altar_utils_log(ALTAR_VERBOSE_LOG, "Reading zip data...");
	zip_int64_t bytes_read, bytes_remaining = stat.size;
	do {
		if ((bytes_read = zip_fread(file, data, stat.size)) == -1)
			altar_utils_error("Error reading zip data: %s", zip_strerror(archive->zip));
	} while (bytes_remaining -= bytes_read);

	zip_fclose(file);
	data[stat.size] = '\0';
	return (struct altar_archive_data) { data, stat.size + 1 };
}

void altar_utils_files_closeArchive(struct altar_archive *archive) {
	if (!archive)
		return;
	altar_utils_log(ALTAR_INFO_LOG, "Closing data archive...");
	zip_close(archive->zip);
	free(archive);
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

