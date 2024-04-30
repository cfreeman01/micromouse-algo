#include "misc.h"

#if defined(_WIN32)

#include <conio.h>

bool isKeyPressed(char c)
{
    return (kbhit() && getch() == c);
}

#elif defined(__linux__)

bool isKeyPressed(char c)
{
    return 0;
}

#endif