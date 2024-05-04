#include "misc.h"

bool keysPressed[256] = {0};

bool isKeyPressed(unsigned char c)
{
    bool ret = keysPressed[c];
    keysPressed[c] = 0;
    return ret;
}