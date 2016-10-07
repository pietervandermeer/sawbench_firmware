#include "SimpleEnvelope.h"

//--
// envelope generation, including adsr and such
//--

#ifdef UNITTEST
const uint16_t stepLut[128] =
#else
#include <avr/pgmspace.h>
PROGMEM const uint16_t stepLut[128] =
#endif
{
// FW1.2 : for 9 bit precision
0xFFFF,
0xCB6F,
0xA455,
0x86A4,
0x6FB1,
0x5DAD,
0x4F56,
0x43C7,
0x3A5D,
0x329D,
0x2C2D,
0x26C9,
0x223D,
0x1E60,
0x1B12,
0x183B,
0x15C6,
0x13A3,
0x11C6,
0x1023,
0x0EB2,
0x0D6B,
0x0C4A,
0x0B48,
0x0A61,
0x0993,
0x08D9,
0x0831,
0x079A,
0x0711,
0x0694,
0x0623,
0x05BC,
0x055D,
0x0506,
0x04B7,
0x046E,
0x042B,
0x03ED,
0x03B3,
0x037E,
0x034D,
0x0320,
0x02F5,
0x02CE,
0x02A9,
0x0287,
0x0267,
0x0249,
0x022D,
0x0212,
0x01FA,
0x01E3,
0x01CD,
0x01B8,
0x01A5,
0x0193,
0x0182,
0x0172,
0x0162,
0x0154,
0x0146,
0x0139,
0x012D,
0x0121,
0x0116,
0x010C,
0x0102,
0x00F8,
0x00EF,
0x00E7,
0x00DF,
0x00D7,
0x00CF,
0x00C8,
0x00C1,
0x00BB,
0x00B5,
0x00AF,
0x00A9,
0x00A4,
0x009F,
0x009A,
0x0095,
0x0090,
0x008C,
0x0088,
0x0084,
0x0080,
0x007C,
0x0078,
0x0075,
0x0072,
0x006E,
0x006B,
0x0068,
0x0065,
0x0063,
0x0060,
0x005E,
0x005B,
0x0059,
0x0056,
0x0054,
0x0052,
0x0050,
0x004E,
0x004C,
0x004A,
0x0048,
0x0047,
0x0045,
0x0043,
0x0042,
0x0040,
0x003F,
0x003D,
0x003C,
0x003A,
0x0039,
0x0038,
0x0037,
0x0035,
0x0034,
0x0033,
0x0032,
0x0031,
0x0030
};

#ifdef UNITTEST
inline uint16_t getStep(uint8_t i)
{
  return stepLut[i];
}
#else
inline uint16_t getStep(uint8_t i)
{
  return pgm_read_word_near(stepLut + i);
}
#endif

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

void SimpleEnvelope::setAttack(uint8_t attack)
{
  attackStep = getStep(attack);
  if (expMode)
  {
    attackStep = 0xFFFF - attackStep;
    // attackStep /= 2;
  }
}

void SimpleEnvelope::setDecay(uint8_t decay)
{
  decayStep = getStep(decay);
  if (expMode)
  {
    decayStep = 0xFFFF - decayStep;
    // decayStep /= 2;
  }
}

void SimpleEnvelope::setSustainLevel(uint8_t sustain)
{
  sustainLevel = ((int32_t) sustain)<<precision;
}

void SimpleEnvelope::setRelease(uint8_t release)
{
  releaseStep = getStep(release);
  if (expMode)
  {
    releaseStep = 0xFFFF - releaseStep;
    // releaseStep /= 2;
  }
}

void SimpleEnvelope::stop()
{
  // already stopped? don't release it again! 
  if (adsrState != ADSR_STATE_RELEASE && adsrState != ADSR_STATE_DONE)
  {
    adsrState = ADSR_STATE_RELEASE;
    expPos = releaseStep;
    lastLevel = output;
  }
}

void SimpleEnvelope::setExpMode(bool mode)
{
  if (mode != expMode)
  {
    attackStep = 0xFFFF - attackStep;
    decayStep = 0xFFFF - decayStep;
    releaseStep = 0xFFFF - releaseStep;
  }
  expMode = mode;
}

void SimpleEnvelope::trigger()
{
  adsrState = ADSR_STATE_ATTACK;
  expPos = attackStep;
  output = 0;
  itersDone = 0;
}

#ifdef UNITTEST
#include <stdio.h>
#endif

// determine envelope output from ticks waited
void SimpleEnvelope::run()
{
  if (expMode)
  {
    switch (adsrState)
    {
    case ADSR_STATE_ATTACK:
#ifdef UNITTEST
      printf("attack\n");
#endif
      output = (255L<<precision) - (expPos<<(precision-8));
      if (output < 0)
      {
        output = 0;
      }
      else if (output >= (255L<<precision))
      {
        adsrState = ADSR_STATE_DECAY;
        expPos = decayStep;
        output = 255L<<precision;
      }
      expPos = (expPos * ((uint32_t) attackStep)) / 65536;
      break;
    case ADSR_STATE_DECAY:
#ifdef UNITTEST
      printf("decay\n");
#endif
      output = sustainLevel + (((expPos/256) * ((255L<<precision)-sustainLevel))/256);
      // if (output <= sustainLevel)
      // {
      //   output = sustainLevel;
      //   adsrState = ADSR_STATE_SUSTAIN;
      // }
      expPos = (expPos * ((uint32_t) decayStep)) / 65536;
      break;
//     case ADSR_STATE_SUSTAIN:
// #ifdef UNITTEST
//       printf("sustain\n");
// #endif
//       expPos = releaseStep;
//       if (loopMode && (itersDone > 0))
//       {
//         adsrState = ADSR_STATE_RELEASE;
//       }
//       else
//       {
//         output = sustainLevel;
//       }
//       // printf("sustain\n");
//       break;
    case ADSR_STATE_RELEASE:
#ifdef UNITTEST
      printf("release\n");
#endif
      output = (expPos * (lastLevel>>precision)) >> (16-precision);
      if (output <= 0)
      {
        adsrState = ADSR_STATE_DONE;
      } 
      expPos = (expPos * (uint32_t) releaseStep) / 65536;
      // printf("release\n");
      break;
    case ADSR_STATE_DONE:
      output = 0;
      // printf("done\n");
      if (loopMode)
      {
        adsrState = ADSR_STATE_ATTACK;
        itersDone++;
      }
      break;
    } // switch
  } // if (expMode)
  else 
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
    } // switch
  } // if (expMode)

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
