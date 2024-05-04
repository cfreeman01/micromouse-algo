#include "stateMachine.h"
#include <asf.h>
#include <mouse_control_interface/configswitch_mci.h>

MouseState state = FIRST_TRAVERSAL;

MouseState firstTraversal(void);
MouseState backToStart(void);
MouseState runToGoal(void);
MouseState reset1(void);
MouseState reset2(void);
MouseState goToLastPoint(void);
MouseState finished(void);
void beginBacktrack(void);
void beginRunToGoal(void);
StateTransition* getTransition(MouseState curState, MouseState nextState);

MouseState (*stateFuncs[])(void) = {
	firstTraversal,
	backToStart,
	runToGoal,
	reset1,
	reset2,
	goToLastPoint,
	finished
};

StateTransition stateTransitions[] = {
	{FIRST_TRAVERSAL,  FIRST_TRAVERSAL,  NULL},
	{FIRST_TRAVERSAL,  BACK_TO_START,    beginBacktrack},
	{FIRST_TRAVERSAL,  RESET_1,          resetMouse1},	
	{BACK_TO_START,    BACK_TO_START,    NULL},
	{BACK_TO_START,    RUN_TO_GOAL,      beginRunToGoal},
	{BACK_TO_START,    RESET_2,          resetMouse2},
	{RUN_TO_GOAL,      RUN_TO_GOAL,      NULL},
	{RUN_TO_GOAL,      FINISHED,         NULL},
	{RUN_TO_GOAL,      RESET_2,          resetMouse2},
	{RESET_1,          GO_TO_LAST_POINT, beginGoToLastPoint},
	{RESET_1,          RESET_1,          NULL},
	{GO_TO_LAST_POINT, GO_TO_LAST_POINT, NULL},
	{GO_TO_LAST_POINT, FIRST_TRAVERSAL,  resumeFirstTraversal},
	{RESET_2,          RESET_2,          NULL},
	{RESET_2,          RUN_TO_GOAL,      beginRunToGoal},
	{FINISHED,         FINISHED,         NULL}
};

/* Function definitions */
void algoIterate(void)
{
	MouseState nextState = stateFuncs[state]();
	
	StateTransition* transition = getTransition(state, nextState);
	
	if(transition == NULL)
	{
		//printf("ERROR: Invalid state transition!!!");
		state = FINISHED;
		return;
	}
	
	if(transition->func != NULL)
		transition->func();
	
	state = nextState;
}

MouseState firstTraversal(void)
{
	if(mci_CheckConfigButtonPressed())
		return RESET_1;
	
	if(firstTraversalItr())
		return BACK_TO_START;
		
	return FIRST_TRAVERSAL;
}

MouseState backToStart(void)
{
	if(mci_CheckConfigButtonPressed())
		return RESET_2;
	
	if(backtrackItr())
		return RUN_TO_GOAL;
		
	return BACK_TO_START;
}

MouseState runToGoal(void)
{
	if(mci_CheckConfigButtonPressed())
		return RESET_2;
	
	if(runToGoalItr())
		return FINISHED;
		
	return RUN_TO_GOAL;
}

MouseState reset1(void)
{
	if(mci_CheckConfigButtonPressed())
		return GO_TO_LAST_POINT;
		
	return RESET_1;
}

MouseState reset2(void)
{
	if(mci_CheckConfigButtonPressed())
		return RUN_TO_GOAL;
	
	return RESET_2;	
}

MouseState goToLastPoint(void)
{
	if(goToLastPointItr())
		return FIRST_TRAVERSAL;

	return GO_TO_LAST_POINT;
}

MouseState finished(void)
{
	return FINISHED;
}

StateTransition* getTransition(MouseState curState, MouseState nextState)
{
	unsigned int numTransitions = sizeof(stateTransitions) / sizeof(StateTransition);
	
	for(int i = 0; i < numTransitions; i++)
		if(stateTransitions[i].curState == curState && stateTransitions[i].nextState == nextState)
			return &stateTransitions[i];
	
	return NULL;
}