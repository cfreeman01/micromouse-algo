#if defined(__linux__)
#include "misc.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "mazeReadAndDisplay.h"

static struct termios t_info;
static int old_flags;

int init()
{
    int ret = mazeInit();

    tcgetattr(STDIN_FILENO, &t_info);
    t_info.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_info);

    old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);

    return ret;
}

void end()
{
    t_info.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_info);

    fcntl(STDIN_FILENO, F_SETFL, old_flags);
}

void sleepMs(uint32_t ms)
{
    usleep(ms * 1000);
}

void clearTerminal()
{
    system("clear");
}

void getKeyPressed()
{
    unsigned char c = getchar();

    keysPressed[c] = 1;
}

#endif