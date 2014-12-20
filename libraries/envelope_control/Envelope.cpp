#include "Envelope.h"

// 1: optimize using step (no div).. will break unit tests, 0: just use div
#define SEQUENTIAL_STEP 1

namespace SignalControl
{

inline void Envelope::resetElapsed()
{
#if SEQUENTIAL_STEP
  adsrTicksWaited = 0;
#else
  oldTime = getMicros();
#endif
}

inline uint32_t Envelope::getElapsed()
{
  return getMicros() - oldTime;
}

Envelope::Envelope(uint32_t (*getMicrosPtr)(), uint32_t ticklen, uint32_t timerTicksPerSecond)
{
  getMicros = getMicrosPtr;
  resetElapsed();
  adsrState = ADSR_STATE_DONE;
  adsrTicklen = ticklen;
  this->timerTicksPerSecond = timerTicksPerSecond;
  adsrTicksPerSecond = timerTicksPerSecond / adsrTicklen;
  setAttack(1);
  setRelease(1);
  setDecay(0);
  setSustainLevel(255);
}

// returns 24.8 fixedpoint
inline uint32_t Envelope::midiTime2Seconds(uint8_t midiTime)
{
  return midiTime*8; // 128 -> 4.0 seconds. 2.8 fixedpoint
}

void Envelope::setTimerTicksPerSecond(uint32_t& timerTicksPerSecond)
{
  this->timerTicksPerSecond = timerTicksPerSecond;
}

inline uint32_t Envelope::seconds2AdsrTicks(uint32_t seconds /*24.8 fixedpoint*/)
{
  return (seconds * (timerTicksPerSecond / adsrTicklen))>>8; //10+log2(timerTicksPerSecond)-log2(adsrticklen)-8 = 10+26-14-8=14.. 2^14=16384, 14+8=2^22 (4 mln) timer ticks
}

// two in one..
inline uint32_t Envelope::midiTime2AdsrTicks(uint8_t midiTime)
{
  return (((uint32_t) midiTime) * adsrTicksPerSecond)>>5; //10+log2(timerTicksPerSecond)-log2(adsrticklen)-8 = 10+26-14-8=14.. 2^14=16384, 14+8=2^22 (4 mln) timer ticks
}

void Envelope::setRelease(uint8_t release)
{
  if (release == 0)
  {
    adsrTime.release = 1;
  }
  else
  {
//    adsrTime.release = seconds2AdsrTicks(midiTime2Seconds(release));
    adsrTime.release = midiTime2AdsrTicks(release);
  }
}

void Envelope::setAttack(uint8_t attack)
{
  if (attack == 0)
  {
    adsrTime.attack = 1;
  }
  else
  {
//    adsrTime.attack = seconds2AdsrTicks(midiTime2Seconds(attack));
    adsrTime.attack = midiTime2AdsrTicks(attack);
  }
}

void Envelope::setDecay(uint8_t decay)
{
  if (decay == 0)
  {
    adsrTime.decay = 1;
  }
  else
  {
//    adsrTime.decay = seconds2AdsrTicks(midiTime2Seconds(decay));
    adsrTime.decay = midiTime2AdsrTicks(decay);
  }
}

void Envelope::setSustainLevel(uint8_t sustain)
{
  sustainLevel = ((int32_t) sustain)<<precision;
}

void Envelope::setSustainTime(uint8_t sustain)
{
  if (sustain == 0)
  {
    adsrTime.sustain = 0;
  }
  else
  {
//    adsrTime.sustain = seconds2AdsrTicks(midiTime2Seconds(sustain));
    adsrTime.sustain = midiTime2AdsrTicks(sustain);
  }
}

uint32_t Envelope::getStopTime()
{
  return (adsrTime.attack + adsrTime.decay + adsrTime.sustain + adsrTime.release) * adsrTicklen;
}

uint32_t Envelope::getInterruptedStopTime(uint32_t interruptionTime /* usec */)
{
  return (adsrTime.release * adsrTicklen) + (interruptionTime*(timerTicksPerSecond/1000000)); // assuming timertickspersecond >> 1000000
}

uint32_t Envelope::getAttackTicks()
{
  return adsrTime.attack * adsrTicklen;
}

uint32_t Envelope::getDecayTicks()
{
  return adsrTime.decay * adsrTicklen;
}

void Envelope::Statemachine(bool& triggered, bool& stopped, int16_t& amplitude)
{
  if (triggered) // triggering overrides every state
  {
    resetElapsed();
    triggered = false;
    adsrState = ADSR_STATE_ATTACK;
  }
  if (stopped && adsrState != ADSR_STATE_RELEASE && adsrState != ADSR_STATE_DONE)
  {
    adsrState = ADSR_STATE_RELEASE;
    stopped = false;
    resetElapsed();
  }
#if SEQUENTIAL_STEP
#else
  adsrTicksWaited = (int32_t) (getElapsed() / adsrTicklen);
#endif

  // determine VCA amplitude from ADSR time
  switch (adsrState)
  {
    case ADSR_STATE_ATTACK:
      amplitude = (255<<precision) * adsrTicksWaited / adsrTime.attack;
      accumulatedLevel = amplitude;
      if (adsrTicksWaited >= adsrTime.attack)
      {
        adsrState = ADSR_STATE_DECAY;
      }
      break;
    case ADSR_STATE_DECAY:
      amplitude = sustainLevel + ((((255<<precision)-sustainLevel) * (adsrTime.attack + adsrTime.decay - adsrTicksWaited)) / adsrTime.decay);
      accumulatedLevel = amplitude;
      if (amplitude <= sustainLevel)
      {
        adsrState = ADSR_STATE_SUSTAIN;
      }
      break;
    case ADSR_STATE_SUSTAIN:
      amplitude = sustainLevel;
      accumulatedLevel = sustainLevel;
      if (adsrTime.sustain == 0) // live mode
      {
        break;
      }
      if (adsrTicksWaited >= adsrTime.sustain+adsrTime.decay+adsrTime.attack) // sequencer mode
      {
        resetElapsed();
        adsrState = ADSR_STATE_RELEASE;
      }
      break;
    case ADSR_STATE_RELEASE:
      if (adsrTicksWaited >= adsrTime.release)
      {
        amplitude = 0;
      }
      else
      {
        amplitude = (accumulatedLevel * (adsrTime.release - adsrTicksWaited)) / adsrTime.release;
      }
      if (amplitude <= 0)
      {
        adsrState = ADSR_STATE_DONE;
      } 
      break;
    case ADSR_STATE_DONE:
      amplitude = 0;
      break;
  }

  if (amplitude > 255<<precision) 
  {
    amplitude = 255<<precision;
  }
  else if (amplitude < 0) 
  {
    amplitude = 0;
  }
#if SEQUENTIAL_STEP
  adsrTicksWaited++;
#endif
}

Envelope::~Envelope()
{

} 

} // namespace SignalControl
