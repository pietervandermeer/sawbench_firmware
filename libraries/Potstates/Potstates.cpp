#include "Potstates.h"

				Potstates::Potstates()
				{
					tresholdPot = 6;
				}

				Potstates::~Potstates()
				{

				}

		int	Potstates::potStatesWrite(unsigned char potState_index, int pot_value)
				{
					potMoved = 0;
				 	unsigned char lastPotState_index = potState_index + 3 - 1;
				 	if (lastPotState_index > 3)
				 	{
				 		lastPotState_index -= 3;
				 	}
					if(pot_value > (lastState[lastPotState_index] + tresholdPot) ||
						 pot_value < (lastState[lastPotState_index] - tresholdPot) || potMoved)
						{
							new_pot_value[potState_index] = pot_value;
							potMoved = 1;
						}

					lastState[potState_index] = pot_value;
				return new_pot_value[potState_index];
				}
