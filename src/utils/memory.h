#ifndef ALTAR_UTILS_MEMORY_H
#define ALTAR_UTILS_MEMORY_H

void *altar_malloc(unsigned size);
void altar_free(void *ptr);
void *altar_calloc(unsigned nmemb, unsigned size);
void *altar_realloc(void *ptr, unsigned size);

#endif

