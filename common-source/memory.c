#include "memory.h"

#include <stdint.h>
#include "pd_api.h"

#include "globals.h"

// Allocates heap space if ptr is NULL, 
// else reallocates the given pointer. 
// If size is zero, frees the given pointer.

void *pdMalloc(size_t size) {
    void *blah = pd->system->realloc(NULL, size);

    // Clobber the first couple of bytes - in the simulator
    // pretty often get the first 8 bytes zeroed.
    if (size >= 4) {
        *((int32_t *)blah) = 0xABAD0B0E;
    }

    return blah;
} // pdMalloc


void *pdRealloc(void *memory, size_t size) {
    return pd->system->realloc(memory, size);
} // pdRealloc


void pdFree(void *memory) {
    pd->system->realloc(memory, 0);
} // pdFree
