#include <math.h>
#include "FrequencyModulator.h"

namespace SignalControl
{

FrequencyModulator::FrequencyModulator(
	uint32_t (*getMicrosPtr)(), 
	uint16_t modFrequency, // modulation frequency (integer)
	uint16_t mainloopFreq) // frequency of program main loop (integer
{
    getMicros = getMicrosPtr;
    setFreq(modFrequency, mainloopFreq);
    lutIndex = 0;
    oldTime = getMicros();
#ifndef ARDUINO
	lut = new int8_t [LUT_SIZE];
	for (unsigned int i=0; i<LUT_SIZE; i++)
	{
		lut[i] = 127.9 * cos( (float) i * M_PI * 2. / (float) LUT_SIZE);
	}
#endif
}

FrequencyModulator::~FrequencyModulator()
{
#ifndef ARDUINO
	delete [] lut;
#endif
}

void FrequencyModulator::setLut(int8_t* lut)
{
	this->lut = lut;
}

void FrequencyModulator::setFreq(
	uint16_t modFrequency, // modulation frequency (integer)
	uint16_t mainloopFreq) // frequency of program main loop (integer
{
    // convert modulation frequency to lut step
    lutStep = (((uint32_t) modFrequency * (uint32_t) LUT_SIZE)<<8) / (uint32_t) mainloopFreq;
}

int8_t FrequencyModulator::getAmp()
{
	return lut[getMicros() % LUT_SIZE];
}

int8_t FrequencyModulator::step()
{
	lutIndex += lutStep;
	return lut[(lutIndex>>8) % LUT_SIZE];
}

} // namespace SignalControl
