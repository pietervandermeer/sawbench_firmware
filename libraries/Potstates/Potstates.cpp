#include "Potstates.h"

Potstates::Potstates()
{
	tresholdPot = 6;
	for (int i=0; i<3; i++)
	{
		callbacks[i] = 0;
	}
}

Potstates::~Potstates()
{

}

void Potstates::registerCallback(unsigned char index, void (*callbackPtr)(int))
{
	callbacks[index] = callbackPtr;
}

int	Potstates::potStatesWrite(unsigned char potState_index, int pot_value)
{
	potMoved = 0;
 	char lastPotState_index = potState_index - 1;
 	if (lastPotState_index < 0)
 	{
 		lastPotState_index += 3;
 	}
	if (pot_value > (lastState[lastPotState_index] + tresholdPot) ||
		pot_value < (lastState[lastPotState_index] - tresholdPot) || potMoved)
	{
		new_pot_value[potState_index] = pot_value;
		potMoved = 1;
		if (callbacks[potState_index])
		{
			callbacks[potState_index](pot_value);
		}
	}

	lastState[potState_index] = pot_value;
	return new_pot_value[potState_index];
}
