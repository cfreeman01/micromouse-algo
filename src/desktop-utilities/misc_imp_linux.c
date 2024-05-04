#if defined(__linux__)
#include "misc.h"
#include <unistd.h>

void sleepMs(uint32_t ms)
{
    usleep(ms * 1000);
}

void clearTerminal()
{
    system("clear");
}

#endif