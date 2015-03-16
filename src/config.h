#include <sys/syscall.h>
#include "event_loop.h"

//#define DBG_PRINTF(...)
#define DBG_PRINTF debug_print

#define gettid() syscall(__NR_gettid)

#define MAX_LOOP 5

event_loop *loops[MAX_LOOP];
