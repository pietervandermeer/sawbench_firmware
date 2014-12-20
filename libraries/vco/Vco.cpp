
#include <Vco.h>

/*PROGMEM*/ const uint16_t freq_table[] =
{
// theoretical linearization - taking into account saw discharge time
  100 // C-1
, 106
, 112
, 118
, 126
, 133
, 141
, 149
, 158
, 168
, 178
, 188
, 200 // C0
, 212
, 224
, 238
, 252
, 267
, 283
, 300
, 318
, 336
, 357
, 378
, 400 // C1 - 32.703 Hz
, 424
, 450
, 476
, 505
, 535
, 567
, 601
, 637
, 675
, 715
, 758
, 803 // C2 - 65.406 Hz, 15.29 ms
, 851
, 902
, 956
, 1013
, 1074
, 1138
, 1206
, 1278
, 1355
, 1436
, 1522
, 1614
, 1711
, 1813
, 1922
, 2038
, 2161
, 2291
, 2429
, 2575
, 2731
, 2896
, 3071
, 3257
, 3454
, 3664
, 3886
, 4123
, 4374
, 4640
, 4924
, 5225
, 5545
, 5886
, 6248
, 6633
, 7042
, 7478
, 7942
, 8437
, 8963
, 9524
, 10122
, 10759
, 11439
, 12165
, 12940
, 13767
, 14652
, 15598
, 16610
, 17694
, 18855
, 20100
, 21436
, 22870
, 24413
, 26072
, 27860
, 29788
, 31869
, 34119
, 36555
, 39197
, 42066
, 45188
, 48593
, 52312
, 56386
, 60860
};

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
  else if (vcoType == Vco::VCO_TYPE_WARTHOG)
  {
    inputOffset = 32;
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
