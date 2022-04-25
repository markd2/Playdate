// Place to expand the implementation of single-file libraries.

// So far, just the one
#include "pd_api.h"

#define STB_DS_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
#include "stb_ds.h"
#pragma GCC diagnostic pop
