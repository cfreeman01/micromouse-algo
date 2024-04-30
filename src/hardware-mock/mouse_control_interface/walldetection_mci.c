#include "walldetection_mci.h"
#include <algo/maze.h>
#include <desktop-utilities/mazeReadAndDisplay.h>

mci_wall_presence_t mci_CheckFrontWall(void)
{
    MazeCell curCell = mazeFull[mazeIdx(getCurPoint())];

    switch(getCurDir())
    {
        case NORTH:
            return curCell.northWall;
            break;

        case SOUTH:
            return curCell.southWall;
            break;

        case EAST:
            return curCell.eastWall;
            break;

        case WEST:
            return curCell.westWall;
            break;
    }
}

mci_wall_presence_t mci_CheckLeftWall(void)
{
    MazeCell curCell = mazeFull[mazeIdx(getCurPoint())];

    switch(getCurDir())
    {
        case NORTH:
            return curCell.westWall;
            break;

        case SOUTH:
            return curCell.eastWall;
            break;

        case EAST:
            return curCell.northWall;
            break;

        case WEST:
            return curCell.southWall;
            break;
    }
}

mci_wall_presence_t mci_CheckRightWall(void)
{
    MazeCell curCell = mazeFull[mazeIdx(getCurPoint())];

    switch(getCurDir())
    {
        case NORTH:
            return curCell.eastWall;
            break;

        case SOUTH:
            return curCell.westWall;
            break;

        case EAST:
            return curCell.southWall;
            break;

        case WEST:
            return curCell.northWall;
            break;
    }
}