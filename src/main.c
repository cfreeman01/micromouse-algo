#include <stdio.h>
#include <desktop-utilities/misc.h>
#include <desktop-utilities/mazeReadAndDisplay.h>
#include <algo/stateMachine.h>

#define ESC 27

int main(void)
{
    if (readMazeTxtFromFile("mazes/test.txt", mazeText))
		return 1;

	getMazeCells(mazeText, mazeFull);

    while(!isKeyPressed(ESC))
    {
        algoIterate();

        CLEAR_TERMINAL();

        printMazeTxt(mazeText, getCurPoint());

        SLEEP_MS(75);
    }

    return 0;
}