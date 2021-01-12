#ifndef ALTAR_UTILS_FILES_H
#define ALTAR_UTILS_FILES_H

struct altar_archive;

struct altar_archive_data {
	char *data;
	unsigned size;
};

char *altar_files_getProgramDirectory(void);
char *altar_files_getDataDirectory(void);
char *altar_files_concatPaths(const char *const a, const char *const b);

void altar_files_overwrite(const char *const file_path, const char *const data);
void altar_files_append(const char *const file_path, const char *const data);
void altar_files_delete(const char *const file_path);
void altar_files_compress(const char *const src_file, const char *const dst_file);

struct altar_archive *altar_files_openArchive(const char *const file_path);
struct altar_archive_data altar_files_readFromArchive(struct altar_archive *archive, const char *const file_path);
void altar_files_closeArchive(struct altar_archive *archive);

#endif

