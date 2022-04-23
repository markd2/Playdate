#include "memory.h"

#include "pd_api.h"

#include "globals.h"

// Allocates heap space if ptr is NULL, 
// else reallocates the given pointer. 
// If size is zero, frees the given pointer.

void *pdMalloc(size_t size) {
    return pd->system->realloc(NULL, size);
} // pdMalloc


void *pdRealloc(void *memory, size_t size) {
    return pd->system->realloc(memory, size);
} // pdRealloc


void pdFree(void *memory) {
    pd->system->realloc(memory, 0);
} // pdFree
