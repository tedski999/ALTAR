#include "files.h"
#include "log.h"
#include "error.h"
#include "memory.h"
#include "../core/core.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>
#if ALTAR_PLATFORM == LINUX
#include <sys/stat.h>
#include <unistd.h>
#elif ALTAR_PLATFORM == WINDOWS
#include <windows.h>
#endif

#if ALTAR_PLATFORM == LINUX
#define get_known_directory() ((getenv("XDG_DATA_HOME")) ? strdup(getenv("XDG_DATA_HOME")) : altar_utils_files_concatPaths(getenv("HOME"), ".local/share"))
#define locate_binary(buffer, buffer_size) (readlink("/proc/self/exe", buffer, buffer_size))
#elif ALTAR_PLATFORM == WINDOWS
#define get_known_directory() (_strdup(getenv("LOCALAPPDATA")))
#define mkdir(dir, mode) (mkdir(dir))
#define locate_binary(buffer, buffer_size) (GetModuleFileNameA(NULL, buffer, buffer_size))
#endif

struct altar_archive {
	const char *file_path;
	struct unzFile *zip;
};

static void altar_utils_files_internal_writeToFile(const char *const file_path, const char *const data, const char *const file_mode);
static void altar_utils_files_internal_mkdirForFile(const char *const file_path);

char *altar_utils_files_getProgramDirectory(void) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Locating program directory...");

	// fit path to binary in path_buffer
	int path_buffer_used, path_buffer_size = 64;
	char *path_buffer = altar_malloc(path_buffer_size);
	while ((path_buffer_used = locate_binary(path_buffer, path_buffer_size - 1)) >= path_buffer_size - 2)
		path_buffer = altar_realloc(path_buffer, path_buffer_size *= 2);
	if (path_buffer_used <= 0)
		altar_utils_error("Unable to locate program directory!");

	// truncate binary filename from path
	path_buffer[path_buffer_used] = '\0';
	*strrchr(path_buffer, ALTAR_PATH_DELIMITER) = '\0';
	return path_buffer;
}

char *altar_utils_files_getDataDirectory(void) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Locating user data directory...");
	char *known_directory = get_known_directory();

	// FIXME: append client-defined "<app name>" instead
	char *data_directory = NULL;
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
	sprintf(result, "%s%c%s", a, ALTAR_PATH_DELIMITER, b);
	return result;
}

void altar_utils_files_overwrite(const char *const file_path, const char *const data) {
	altar_utils_files_internal_writeToFile(file_path, data, "w");
}

void altar_utils_files_append(const char *const file_path, const char *const data) {
	altar_utils_files_internal_writeToFile(file_path, data, "a");
}

void altar_utils_files_delete(const char *const file_path) {
	altar_utils_log(ALTAR_VERBOSE_LOG, "Deleting file '%s'...", file_path);
	if (remove(file_path))
		altar_utils_log(ALTAR_WARNING_LOG, "Unable to delete file '%s'!", file_path);
}

void altar_utils_files_compress(const char *const src_file, const char *const dst_file) {
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

	*archive = (struct altar_archive) { file_path, unzOpen(file_path) };
	if (!archive->zip) {
		altar_free(archive);
		altar_utils_error("Could not open archive '%s'!", file_path);
	}

	return archive;
}

struct altar_archive_data altar_utils_files_readFromArchive(struct altar_archive *archive, const char *const file_path) {
	altar_utils_log(ALTAR_INFO_LOG, "Reading '%s' from archive '%s'...", file_path, archive->file_path);

	altar_utils_log(ALTAR_VERBOSE_LOG, "Locating zip data...");
	if (unzLocateFile(archive->zip, file_path, 1) != UNZ_OK)
		altar_utils_error("Unable to locate '%s' within archive '%s'!", file_path, archive->file_path);

	altar_utils_log(ALTAR_VERBOSE_LOG, "Querying zip data...");
	unz_file_info file_info;
	if (unzGetCurrentFileInfo(archive->zip, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
		altar_utils_error("Unable to query '%s' within archive '%s'!", file_path, archive->file_path);

	altar_utils_log(ALTAR_VERBOSE_LOG, "Opening zip data...");
	if (unzOpenCurrentFile(archive->zip) != UNZ_OK)
		altar_utils_error("Unable to open '%s' within archive '%s'!", file_path, archive->file_path);

	altar_utils_log(ALTAR_VERBOSE_LOG, "Reading zip data...");
	char *data = altar_malloc(file_info.uncompressed_size);
	if (unzReadCurrentFile(archive->zip, data, file_info.uncompressed_size) != 0)
		altar_utils_error("Unable to properly read '%s' within archive '%s'!", file_path, archive->file_path);
	data[file_info.uncompressed_size - 1] = '\0';

	unzCloseCurrentFile(archive->zip);
	return (struct altar_archive_data) { data, file_info.uncompressed_size };
}

void altar_utils_files_closeArchive(struct altar_archive *archive) {
	if (!archive)
		return;
	altar_utils_log(ALTAR_INFO_LOG, "Closing archive '%s'...", archive->file_path);
	unzClose(archive->zip);
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
		if (*ptr == ALTAR_PATH_DELIMITER) {
			*ptr = '\0';
			mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			*ptr = ALTAR_PATH_DELIMITER;
		}
	}

	altar_free(directory);
}

