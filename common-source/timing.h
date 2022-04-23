
#define TIMING_START do { \
    pd->system->resetElapsedTime(); \
    float $then = pd->system->getElapsedTime(); \


#define TIMING_END \
    float $now = pd->system->getElapsedTime(); \
    print("elapsed time \(%f)", (double)($now - $then)); \
} while(0);




