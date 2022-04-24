#ifndef BORK_TIMING_H
#define BORK_TIMING_H

#define TIMING_START do { \
    pd->system->resetElapsedTime(); \
    float $then = pd->system->getElapsedTime(); \


#define TIMING_END \
    float $now = pd->system->getElapsedTime(); \
    print("elapsed time \(%f)", (double)($now - $then)); \
} while(0);

#endif // BORK_TIMING_H
