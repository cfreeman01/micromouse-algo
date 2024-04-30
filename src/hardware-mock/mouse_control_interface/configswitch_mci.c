#include <desktop-utilities/misc.h>
#include "configswitch_mci.h"

mci_button_pressed_t mci_CheckConfigButtonPressed(void)
{
    if(isKeyPressed('r'))
        return MCI_BUTTON_PRESSED;

    MCI_BUTTON_NOT_PRESSED;
}