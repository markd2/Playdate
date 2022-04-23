
// used to see what the block-zeroing behavior is.
// As of 4/23/2022, simulator pretty reliably (But not 100%) had
// the first 8 bytes zeroed for allocated blocks.  The device always
// had garbage there.
void checkMemoryAssumptions(void) {
    const int blockCount = 100;
    const int blockSize = 5131;
    
    char *blocks[blockCount];

    for (int i = 0; i < blockCount; i++) {
        char *blah = pdMalloc(blockSize);
        blocks[i] = blah;
    }

    for (int i = 0; i < blockCount; i++) {
        // see if any non-zero bytes
        char *scan = blocks[i];
        char *stop = scan + blockSize;

        while (scan < stop) {
            if (*scan != 0) {
                print("block %d has a non-zero vaule '%c' at byte %d",
                      i, *scan, scan - blocks[i]);
                break;
            }
            scan++;
        }
    }
} // checkMemoryAssumptions
