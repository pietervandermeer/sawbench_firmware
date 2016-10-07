#ifndef SIMPLEENVELOPE_LIB_H
#define SIMPLEENVELOPE_LIB_H

#include <stdint.h>

class SimpleEnvelope
{
public:
	static const int precision = 9; // 11; // bits behind the comma

	SimpleEnvelope();
	~SimpleEnvelope();
	void run();
	void setRelease(uint8_t release);
	void setAttack(uint8_t attack);
	void setDecay(uint8_t decay);
	void setSustainLevel(uint8_t sustain);
	void trigger();
	void stop();
	void setLooping(bool mode);
	void setExpMode(bool mode);

	bool loopMode, expMode;
	uint16_t itersDone;
	int32_t output;
	uint32_t expPos, lastLevel;
	int32_t sustainLevel;
	uint16_t attackStep;
	uint16_t decayStep;
	uint16_t releaseStep;

protected:

	enum AdsrStateType 
	{
	  ADSR_STATE_ATTACK, ADSR_STATE_DECAY, ADSR_STATE_SUSTAIN, ADSR_STATE_RELEASE, ADSR_STATE_DONE
	};

	AdsrStateType adsrState;

}; // class SimpleEnvelope

#endif //SIMPLEENVELOPE_LIB_H
