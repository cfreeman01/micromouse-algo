#include "mazeReadAndDisplay.h"
#include <stdio.h>
#include <stdlib.h>

char mazeText[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1];
MazeCell mazeFull[MAZE_LENGTH * MAZE_LENGTH];

int mazeInit()
{
	if (readMazeTxtFromFile("mazes/test.txt", mazeText))
		return 1;

	getMazeCells(mazeText, mazeFull);
}

int readMazeTxtFromFile(char *srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1])
{
	FILE *file = fopen(srcFilename, "r");
	if (file == NULL)
	{
		printf("Could not open maze file!!!\n");
		return 1;
	}

	for (int i = 0; i < MAZE_LENGTH_TXT; i++)
	{
		fgets(destMazeTxt[i], MAZE_LENGTH_TXT + 1, file);
		fgetc(file);
	}

	return 0;
}

int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell *destMaze)
{
	for (int i = 0; i < MAZE_LENGTH; i++)
	{
		for (int j = 0; j < MAZE_LENGTH; j++)
		{
			int mazei = 2 * i;
			int mazej = 2 * j;
			bool northWall = srcMazeTxt[mazei][mazej + 1] == ' ' ? FALSE : TRUE;
			bool southWall = srcMazeTxt[mazei + 2][mazej + 1] == ' ' ? FALSE : TRUE;
			bool eastWall = srcMazeTxt[mazei + 1][mazej + 2] == ' ' ? FALSE : TRUE;
			bool westWall = srcMazeTxt[mazei + 1][mazej] == ' ' ? FALSE : TRUE;
			MazeCell mazeCell = {northWall, southWall, eastWall, westWall};
			destMaze[(MAZE_LENGTH * i) + j] = mazeCell;
		}
	}

	return 0;
}

void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], Point point)
{
	int xTxt, yTxt;

	if (isInRange(point))
	{
		xTxt = (2 * point.x) + 1;
		yTxt = (2 * mirrorY(point.y)) + 1;
	}
	else
	{
		xTxt = -1;
		yTxt = -1;
	}

	for (int i = 0; i < MAZE_LENGTH_TXT; i++)
	{
		for (int j = 0; j < MAZE_LENGTH_TXT; j++)
		{
			if (i == yTxt && j == xTxt)
				printf("*");
			else
				printf("%c", srcMazeTxt[i][j]);
		}
		printf("\n");
	}
}

void printMazeCells(MazeCell *srcMaze)
{
	for (int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++)
	{
		MazeCell mc = srcMaze[i];

		if (i % MAZE_LENGTH == 0)
			printf("\n");

		printf("%c%c%c%c ",
			   mc.northWall ? 'N' : '0',
			   mc.southWall ? 'S' : '0',
			   mc.eastWall ? 'E' : '0',
			   mc.westWall ? 'W' : '0');
	}
}

void printMazeFlood(unsigned int *srcMazeFlood)
{
	for (int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++)
	{
		if (i % MAZE_LENGTH == 0)
			printf("\n");

		printf("%-11u", srcMazeFlood[i]);
	}
}