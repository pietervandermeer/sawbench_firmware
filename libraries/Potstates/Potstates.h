/*==================================================================
Potstates Class by Koen Pepping

This class was made to save the potmeter values from the sawbench, 
in each potstate. While scrolling trough the states you can change
parameters of the envelopes/LFO with the same 4 potmeters.


*///================================================================

class Potstates
{
		public:

				Potstates();
				~Potstates();

		int potStatesWrite(unsigned char potState_index, int pot_value);

		private:
	
				unsigned char tresholdPot;	
				int lastState[3];
				int new_pot_value[3];
				unsigned char potMoved;

};
