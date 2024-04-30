#include "stack.h"

char pop(Direction *stack, unsigned int *top)
{
	if (*top == 0)
	{
		//printf("ERROR: Popping empty stack!\n\r");
		return '\0';
	}

	(*top)--;
	return stack[*top];
}

void push(Direction *stack, unsigned int *top, char data)
{
	if (*top == STACK_SIZE)
	{
		//printf("ERROR: Pushing full stack!\n\r");
		return;
	}

	stack[*top] = data;
	(*top)++;
}