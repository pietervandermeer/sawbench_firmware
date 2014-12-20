#include <stdint.h>

namespace SignalControl
{

class FrequencyModulator
{
public:

	FrequencyModulator(	
		uint32_t (*getMicrosPtr)(), 
		uint16_t modFrequency, // modulation frequency (integer)
		uint16_t mainloopFreq // frequency of program main loop (integer
    );
	~FrequencyModulator();
	void setFreq(uint16_t modFrequency, uint16_t mainloopFreq);
	int8_t getAmp();
	int8_t step();
	void setLut(int8_t* lut);

protected:

	uint32_t (*getMicros)();

	uint32_t oldTime; // usec
	uint32_t lutStep; // 24 bit integer, 8 bit fraction
	uint32_t lutIndex; // 24 bit interger, 8 bit fraction 

	static const uint32_t LUT_SIZE = 256;
	int8_t *lut;
};

} // namespace SignalControl
