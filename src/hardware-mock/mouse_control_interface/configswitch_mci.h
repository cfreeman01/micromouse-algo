#pragma once

typedef enum 
{
    MCI_BUTTON_NOT_PRESSED = 0u,
    MCI_BUTTON_PRESSED
} mci_button_pressed_t;

mci_button_pressed_t mci_CheckConfigButtonPressed(void);