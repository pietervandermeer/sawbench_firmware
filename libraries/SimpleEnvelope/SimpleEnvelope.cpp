#include "SimpleEnvelope.h"

SimpleEnvelope::SimpleEnvelope()
{
  adsrState = ADSR_STATE_DONE;
  setAttack(1);
  setRelease(1);
  setDecay(0);
  setSustainLevel(255);
}

SimpleEnvelope::~SimpleEnvelope()
{
} 

void SimpleEnvelope::setLooping(bool mode)
{
  loopMode = mode;
}

void SimpleEnvelope::setRelease(uint8_t release)
{
  releaseStep = getStep(release);
}

void SimpleEnvelope::setAttack(uint8_t attack)
{
  attackStep = getStep(attack);
}

void SimpleEnvelope::setDecay(uint8_t decay)
{
  decayStep = getStep(decay);
}

void SimpleEnvelope::setSustainLevel(uint8_t sustain)
{
  sustainLevel = ((int32_t) sustain)<<precision;
}

void SimpleEnvelope::stop()
{
  // already stopped? don't release it again! 
  if (adsrState != ADSR_STATE_RELEASE && adsrState != ADSR_STATE_DONE)
  {
    adsrState = ADSR_STATE_RELEASE;
  }
}

void SimpleEnvelope::trigger()
{
  adsrState = ADSR_STATE_ATTACK;
  output = 0;
  itersDone = 0;
}

// determine envelope output from ticks waited
void SimpleEnvelope::run()
{
  switch (adsrState)
  {
    case ADSR_STATE_ATTACK:
      output += attackStep;
      if (output >= (255L<<precision))
      {
        adsrState = ADSR_STATE_DECAY;
        output = 255L<<precision;
      }
      break;
    case ADSR_STATE_DECAY:
      output -= decayStep;
      if (output <= sustainLevel)
      {
        output = sustainLevel;
        adsrState = ADSR_STATE_SUSTAIN;
      }
      break;
    case ADSR_STATE_SUSTAIN:
      if (loopMode && (itersDone > 0))
      {
        adsrState = ADSR_STATE_RELEASE;
      }
      else
      {
        output = sustainLevel;
      }
      break;
    case ADSR_STATE_RELEASE:
      output -= releaseStep;
      if (output <= 0)
      {
        adsrState = ADSR_STATE_DONE;
      } 
      break;
    case ADSR_STATE_DONE:
      output = 0;
      if (loopMode)
      {
        adsrState = ADSR_STATE_ATTACK;
        itersDone++;
      }
      break;
  }

  // clip it
  if (output > 255L<<precision) 
  {
    output = 255L<<precision;
  }
  else if (output < 0) 
  {
    output = 0;
  }

}

