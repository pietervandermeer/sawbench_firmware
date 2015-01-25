#include "Potstates.h"

Potstates::Potstates()
{
	tresholdPot = 16;
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
	if (pot_value > (lastVal + tresholdPot) ||
		pot_value < (lastVal - tresholdPot) )
	{
		new_pot_value[potState_index] = pot_value;
		lastVal = pot_value;
		if (callbacks[potState_index])
		{
			callbacks[potState_index](pot_value);
		}
	}

	return new_pot_value[potState_index];
}
