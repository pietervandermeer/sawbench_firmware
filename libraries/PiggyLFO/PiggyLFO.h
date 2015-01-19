/*=====================================================
LFO class by Koen Pepping

This LFO contains look up tables for:
Sine wave
Saw wave
Square wave

It also contains a random generator for a Sample and hold kind of LFO

This class was made for a PWM output of an Arduino. The output should be RC filtered.
*///===================================================


#ifndef PIGGY_LFO_H
#define PIGGY_LFO_H
#endif

#include "math.h"

#define BUFFERSIZE 256
#define SAMPLERATE 1000.0
#define TABLEFREQUENCY (1000.0 / 256.0)

class LFO
{
	public:

		LFO();
		~LFO();

		int LFOout(int wave, int index);	//output. wave: 0=S&H, 1=sine, 2=saw, 3=square
			
	private:
		int lastIndex;
		int output;						//output to PWM (int between 0-255)
		unsigned long Xrand;


};


