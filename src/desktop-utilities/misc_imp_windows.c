#if defined(_WIN32)
#include "misc.h"
#include <conio.h>
#include <windows.h>
#include "mazeReadAndDisplay.h"

int init()
{
    return mazeInit();
}

void end()
{

}

void sleepMs(uint32_t ms)
{
    Sleep(ms);
}

void clearTerminal()
{
    system("cls");
}

void getKeyPressed()
{
    unsigned char c;
    if(kbhit())
        c = getch();
    else
        return;

    keysPressed[c] = 1;
}

#endif