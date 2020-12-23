#ifndef ALTAR_UTILS_FILES_H
#define ALTAR_UTILS_FILES_H

char *altar_utils_files_getDataDirectory(void);
char *altar_utils_files_concatPaths(const char *const a, const char *const b);
void altar_utils_files_overwrite(const char *const file_path, const char *const data);
void altar_utils_files_append(const char *const file_path, const char *const data);
void altar_utils_files_compress_gz(const char *const src_file, const char *const dst_file);

#endif

