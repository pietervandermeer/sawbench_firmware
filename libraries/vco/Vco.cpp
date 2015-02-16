
#include <Vco.h>

// linearized table..
/*PROGMEM*/ uint16_t freq_table[] =
{
#include "vcotable.h"
};

const uint16_t nrOctaves = 9;
const uint16_t notesPerOctave = 2;
const uint16_t nrCalibNotes = nrOctaves * notesPerOctave;
const uint32_t maxCalibSamples = 1000000;

// TODO pgm mem or generated directly
const uint16_t dac_input[] = 
{
196,
277,
392,
555,
785,
1110,
1570,
2220,
3140,
4440,
6279,
8880,
12558,
17760,
25116,
35519,
50232,
65535
};

float f_measured[18];

void Vco::calibrate()
{

  //
  // measure the frequencies of all input dac values..  
  //

  const int inputPin = 4;
  pinMode(inputPin, INPUT);

  pinMode(8,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
  digitalWrite(8,0);
  digitalWrite(A4,0);
  digitalWrite(A5,0);

  const uint16_t* dac_inputPtr;
  dac_inputPtr = dac_input;
  int i = 0;

  for (uint16_t idx_octave = 0; idx_octave < nrOctaves; idx_octave++)
  {
    for (uint16_t idx_note = 0; idx_note < notesPerOctave; idx_note++)
    {
      // set dac value
      uint16_t dacValue = (*dac_inputPtr++) - 79;
      analogWrite(msPwmPin, (dacValue>>8)&0xFF);
      analogWrite(lsPwmPin, dacValue&0xFF);

      //delay(100); // unnecessary since we always measure between falling edges using the newly configured slope.

      bool bit = PIND & 0b00010000; // arduino pin 4
      //bool bit = PINB & 0b00000001; // arduino pin 8
      bool lastBit = bit;
      uint16_t edgesMeasured = 0; // = periods measured + 1
      uint32_t startTime;
      uint32_t lastEdge = 0;

      // sample the thing at super high speed. don't store anything, just check for falling edges
      for (uint32_t idx_sample = 0; idx_sample < maxCalibSamples; idx_sample++)
      {
        //bool sample = digitalRead(inputPin);
        // optimized hardcoded
        bit = PIND & 0b00010000; // arduino pin 4
        //bit = PINB & 0b00000001; // arduino pin 8

        if (!bit && lastBit && (idx_sample - lastEdge > 50) )
        {
          if (edgesMeasured == 0)
          {
            startTime = micros();
          }
          // falling edge!
          edgesMeasured++;
          lastEdge = idx_sample;
          // the higher the octave the more periods we need to have to remain accurate.. micros() really adds an offset.. 
          // also, we don't want this to take forever for the low octaves.. the user is waiting for the synth to boot.. 
          if ((edgesMeasured > 128) || (edgesMeasured == (uint16_t) (1<<idx_octave)+32))
          {
            break;
          }
        }

        lastBit = bit;
      } // for sample

      uint32_t elapsedTime = micros() - startTime;
      uint32_t measuredPeriod = elapsedTime / (edgesMeasured - 1);
      Serial.println(measuredPeriod);
      f_measured[i++] = 64000000. / (float) measuredPeriod;
//      Serial.print(elapsedTime); Serial.print("/"); Serial.println(edgesMeasured);

    } // for note in octave
  } // for octave

  //
  // now compute the piecewise linear dac->frequency response, and use this to generate the calibration table.
  //

  const int highest_note = 104;
  int j = 0;
  for (int i=12; i<highest_note; i++)
  {
    float f_synth = 8.1757989156 * pow(2.,i/12.); 
    if (f_synth >= f_measured[j+1])
    {
      j++;
    }
    float df = f_measured[j+1] - f_measured[j];
    float ddac = dac_input[j+1] - dac_input[j];
    float f_frac = (f_synth - f_measured[j])/df;
    float dac_synth_ = dac_input[j] + f_frac*ddac;
    uint16_t dac_synth__ = (uint16_t) (dac_synth_ + 0.5);
    Serial.println(dac_synth__);
    freq_table[i] = dac_synth__;
  }

}

// set the voltage that the LT1013 (IC1A) can get closest to GND. on piggy this is 5 mV (66), on superpigy 32 (2.5 mV)
void Vco::setType(Vco::VcoType vcoType)
{
  if (vcoType == Vco::VCO_TYPE_PIGGY)
  {
    inputOffset = 66;
    vcoMode = VCO_MODE_SAW; // it's the only one we know..
  }
  else if (vcoType == Vco::VCO_TYPE_SUPERPIGGY)
  {
    inputOffset = 32;
  }
  else if (vcoType == Vco::VCO_TYPE_SAWBENCH)
  {
    inputOffset = 79;
  }
}

uint16_t Vco::getNotePitch(uint8_t midi_note)
{
	return freq_table[midi_note];
}

void Vco::write(uint16_t synth_pitch)
{
  synth_pitch -= inputOffset; // superpiggy: 2.5 mV
  //synth_pitch = 0; // test to measure offset voltage
	analogWrite(msPwmPin, (synth_pitch>>8)&0xFF);
	analogWrite(lsPwmPin, synth_pitch&0xFF);
}

void Vco::switchVcoMode()
{
  if (vcoMode == VCO_MODE_SAW)
  {
    vcoMode = VCO_MODE_SQUARE;
  }
  else
  {
    vcoMode = VCO_MODE_SAW;
  }
}
