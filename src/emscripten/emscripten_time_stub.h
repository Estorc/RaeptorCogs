#ifdef __EMSCRIPTEN__
#include <time.h>

static inline int clock_gettime(int clk_id, struct timespec *tp) {
    // You can fill tp with 0, or leave it uninitialized
    if (tp) {
        tp->tv_sec = 0;
        tp->tv_nsec = 0;
    }
    return 0;
}
#endif