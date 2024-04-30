#pragma once

typedef enum
{
    MCI_WALL_NOT_FOUND = 0u,
    MCI_WALL_FOUND,
} mci_wall_presence_t;

mci_wall_presence_t mci_CheckFrontWall(void);
mci_wall_presence_t mci_CheckLeftWall(void);
mci_wall_presence_t mci_CheckRightWall(void);