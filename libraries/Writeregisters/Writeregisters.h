/*==================================================================
WriteRegister Class by Koen Pepping

This class is used to make the LED's on the sawbench synth 
indicate:

Which LFO waveform is selected.

Which Pot state is selected.

The arguments LFO_index and POT_index should get their index from 
the scrollbutton class reading out two hardware buttons using arduino. 

*///================================================================

#define NUM_OF_REGISTERS 8 

class Writeregister
{
	public:

		Writeregister();
		~Writeregister();

		void LedWriter(unsigned char numOfPotstates, unsigned char LFO_index, unsigned char	POT_index);

		int ds_PIN;     //Serial port input 74HC595
		int shcp_PIN;   //Shift clock input 74HC595
		int stcp_PIN;   //storage clock input 74HC595 

	private:

		unsigned char registers[NUM_OF_REGISTERS];



};
