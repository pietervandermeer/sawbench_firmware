#ifndef ENVELOPE_LIB_H
#define ENVELOPE_LIB_H

#include <Arduino.h>
#include <stdint.h>

namespace SignalControl
{

//--
// envelope generation, including adsr and such
//--

class Envelope
{
public:
	static const int precision = 6; // bits behind the comma

	Envelope(uint32_t (*getMicrosPtr)(), uint32_t ticklen, uint32_t timerTicksPerSecond);
	~Envelope();
	void resetElapsed();
	uint32_t getElapsed();
	void Statemachine(bool& triggered, bool& stopped, int16_t& amplitude);
	void setRelease(uint8_t release);
	void setAttack(uint8_t attack);
	void setDecay(uint8_t decay);
	void setSustainLevel(uint8_t sustain);
	void setSustainTime(uint8_t sustain);
	uint32_t getStopTime();
	uint32_t getInterruptedStopTime(uint32_t interruptionTime);
	uint32_t midiTime2Seconds(uint8_t midiTime);
	uint32_t seconds2AdsrTicks(uint32_t seconds /*24.8 fixedpoint*/);
	uint32_t midiTime2AdsrTicks(uint8_t midiTime);

	void setTimerTicksPerSecond(uint32_t& timerTicksPerSecond);
	uint32_t getAttackTicks();
	uint32_t getDecayTicks();

protected:
	uint32_t (*getMicros)();

	// ADSR struct, numbers are in "adsr ticks". 
	struct AdsrType
	{
	  int32_t attack;
	  int32_t decay;
	  int32_t sustain;
	  int32_t release;

	  AdsrType(int32_t attack=0, int32_t decay=0, int32_t sustain=0, int32_t release=0)
	  {
	  	this->attack = attack;
	  	this->decay = decay;
	  	this->sustain = sustain;
	  	this->release = release;
	  }
	};

	AdsrType adsrTime;

	uint32_t timerTicksPerSecond; // timerticks/s
	int32_t sustainLevel;
	uint32_t accumulatedLevel;
	uint32_t adsrTicklen; // in timerticks
	uint32_t adsrTicksPerSecond;
	uint32_t adsrTicksWaited;

	enum AdsrStateType 
	{
	  ADSR_STATE_ATTACK, ADSR_STATE_DECAY, ADSR_STATE_SUSTAIN, ADSR_STATE_RELEASE, ADSR_STATE_DONE
	};

	AdsrStateType adsrState;

	uint32_t oldTime;

}; // class Envelope

} // namespace SignalControl

#endif //ENVELOPE_LIB_H
