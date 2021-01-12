#include "memory.h"
#include "error.h"
#include <stdlib.h>

void *altar_malloc(unsigned size) {
	void *ptr = malloc(size);
	if (!ptr)
		altar_error_fatal();
	return ptr;
}

void altar_free(void *ptr) {
	free(ptr);
}

void *altar_calloc(unsigned nmemb, unsigned size) {
	void *ptr = calloc(nmemb, size);
	if (!ptr)
		altar_error_fatal();
	return ptr;
}

void *altar_realloc(void *ptr, unsigned size) {
	void *new_ptr = realloc(ptr, size);
	if (!ptr)
		altar_error_fatal();
	return new_ptr;
}

