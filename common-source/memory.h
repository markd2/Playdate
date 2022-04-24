#ifndef BORK_MEMORY_H
#define BORK_MEMORY_H

#include <stdlib.h>

void *pdMalloc(size_t size);

void *pdRealloc(void *memory, size_t size);

void pdFree(void *memory);

#endif // BORK_MEMORY_H
