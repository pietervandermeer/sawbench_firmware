#include <iostream>
#include <cmath>
#include "Envelope.h"

// tick = 1 us / 64
#define TIMERTICKS_PER_MICRO 64

uint32_t micros;

uint32_t getfakeTicks()
{
  return micros*TIMERTICKS_PER_MICRO;
}

SignalControl::Envelope env(&getfakeTicks, (uint16_t) 20000 /* adsr ticklen */, (uint32_t) 64000000UL /*timertickspersec*/);

short am_amp, mod_freq = 30;
unsigned char mod_index;

// test if sharp envelope edges are possible (no div by zero).
void testZeroEdges()
{
  env.setSustainTime(1);
  env.setSustainLevel(128);
  env.setAttack(0);
  env.setRelease(0);
}

bool testEnvelopeShape(uint8_t attackTime, uint8_t decayTime, uint8_t sustainTime, uint8_t releaseTime, uint8_t sustainLevel, uint32_t interruptionTime /* usec */)
{
  int16_t amp;
  bool triggered = true;
  bool stopped = false;
  bool peakTestDone = false;
  bool sustainTestDone = false;
  bool interruptionDone = false;
  uint32_t step = 1000; // 1000 usec

  env.setSustainTime(sustainTime); // 0: user interrupted, else 
  env.setSustainLevel(sustainLevel);
  env.setAttack(attackTime);
  env.setDecay(decayTime); 
  env.setRelease(releaseTime);
 
  uint32_t stopTime;
  if (interruptionTime != 0)
  {
    stopTime = env.getInterruptedStopTime(interruptionTime);
    if (interruptionTime*TIMERTICKS_PER_MICRO < env.getAttackTicks())
    {
      peakTestDone = true; // no need to test if peak is reached when we may interrupt it beforehand..
    }
    if (interruptionTime*TIMERTICKS_PER_MICRO < env.getAttackTicks()+env.getDecayTicks())
    {
      sustainTestDone = true; // no need to test if peak is reached when we may interrupt it beforehand..
    }
  }
  else
  {
    stopTime = env.getStopTime();
  }

  for (micros = 0; getfakeTicks() <= stopTime; micros += step)
  {
    // interrupt when caller requested this
    if (interruptionTime != 0 && micros >= interruptionTime && interruptionDone == false) 
    {
      stopped = true; // stopped is reset to false by the statemachine, so we have to use a different variable here.
      interruptionDone = true;
    }

    env.Statemachine(triggered, stopped, amp);
    std::cout << "triggered=" << triggered << " amp=" << amp << " micros=" << micros << "\n";

    // test peak value
    if ((getfakeTicks() >= env.getAttackTicks()) && !peakTestDone)
    {
      peakTestDone = true;
      if (amp < (255<<env.precision) * .99)
      {
        std::cout << "peak value not reached!\n";
        return false;
      }
      else
      {
        std::cout << "peak value ok\n";
      }
    }

    // test sustain value
    if ((getfakeTicks() >= env.getAttackTicks()+env.getDecayTicks()) && !sustainTestDone)
    {
      sustainTestDone = true;
      if ( (amp < (((uint32_t)sustainLevel)<<env.precision) * .99) || 
           (amp > (((uint32_t)sustainLevel)<<env.precision) * 1.01) )
      {
        std::cout << "sustain value not reached!\n";
        return false;
      }
      else
      {
        std::cout << "sustain value ok\n";
      }
    }
  }

  // final value will need to be within chosen relative tolerance..
  uint32_t numberOfSteps = stopTime / (step*TIMERTICKS_PER_MICRO);
  // tolerance of two steps 
//  int32_t limit = (sustainLevel<<SignalControl::Envelope::precision)*2 / numberOfSteps;
  // 1% tolerance
  int32_t limit = (((uint32_t) sustainLevel)<<SignalControl::Envelope::precision) / 100;
  std::cout << "numberOfSteps = " << numberOfSteps << ", limit = " << limit << "\n";

  if (amp > limit)
  {
    std::cout << "final value not close enough to zero!\n";
    return false;
  }

  // check if it stays down!
  micros += step * 10;
  env.Statemachine(triggered, stopped, amp);
  std::cout << "triggered=" << triggered << " amp=" << amp << " micros=" << micros << "\n";
  if (amp != 0)
  {
    std::cout << "values after envelope not zero!\n";
    return false;
  }

  return true;
}

//
// main
//

int main()
{
  int ret = 0;

  testZeroEdges();
  // no crash? = no div by zero, yay!

  // interrupt during sustain
  if (!testEnvelopeShape(2, 2, 3, 4, 128, 80*1000))
  {
    std::cout << "interrupted sustained envelope failed!\n";
    ret &= -1;
  }
  else 
  {
    std::cout << "interrupted sustained envelope ok\n";
  }
  std::cout << "\n\n\n";

  // interrupt before peak
  if (!testEnvelopeShape(2, 2, 3, 4, 128, 30*1000))
  {
    std::cout << "interrupted attack failed!\n";
    ret &= -1;
  }
  else 
  {
    std::cout << "interrupted attack ok\n";
  }
  std::cout << "\n\n\n";

  // test default shape (sustain level at max)
  if (!testEnvelopeShape(1, 1, 1, 1, 255, 0))
  {
    std::cout << "programmed sustained envelope (255) failed!\n";
    ret &= -1;
    return ret;
  }
  else 
  {
    std::cout << "programmed sustained envelope (255) ok\n";
  }
  std::cout << "\n\n\n";

  // create envelopes from miniscule to maximum possible length.. test them all
  for (int scalar=0; scalar<128; scalar++)
  {
    if (!testEnvelopeShape(scalar, scalar, scalar, scalar, 128, 0))
    {
      std::cout << "programmed sustained envelope failed!\n";
      ret &= -1;
      return ret;
    }
    else 
    {
      std::cout << "programmed sustained envelope ok\n";
    }
  }

  return ret;
}
