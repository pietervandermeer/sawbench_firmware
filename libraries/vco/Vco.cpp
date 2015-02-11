
#include <Vco.h>

/*PROGMEM*/ const uint16_t freq_table[] =
{
// theoretical linearization - taking into account saw discharge time
37,
44,
51,
59,
67,
76,
85,
95,
105,
116,
128,
140,
153,
167,
182,
197,
214,
231,
249,
269,
290,
312,
335,
359,
386,
413,
442,
474,
506,
541,
578,
617,
659,
703,
749,
799,
851,
907,
965,
1028,
1094,
1164,
1238,
1317,
1400,
1489,
1582,
1682,
1787,
1897,
2011,
2128,
2249,
2374,
2504,
2638,
2779,
2926,
3082,
3247,
3423,
3611,
3815,
4035,
4273,
4533,
4812,
5108,
5423,
5756,
6111,
6488,
6888,
7313,
7764,
8244,
8755,
9297,
9874,
10488,
11142,
11837,
12577,
13365,
14205,
15100,
16054,
17071,
18156,
19315,
20552,
21874,
23287,
24798,
26416,
28148,
30005,
31996,
34133,
36430,
38899,
41557,
44422,
47513,
50851,
54462,
58374,
62619
//   100 // C-1
// , 106
// , 112
// , 118
// , 126
// , 133
// , 141
// , 149
// , 158
// , 168
// , 178
// , 188
// , 200 // C0
// , 212
// , 224
// , 238
// , 252
// , 267
// , 283
// , 300
// , 318
// , 336
// , 357
// , 378
// , 400 // C1 - 32.703 Hz
// , 424
// , 450
// , 476
// , 505
// , 535
// , 567
// , 601
// , 637
// , 675
// , 715
// , 758
// , 803 // C2 - 65.406 Hz, 15.29 ms
// , 851
// , 902
// , 956
// , 1013
// , 1074
// , 1138
// , 1206
// , 1278
// , 1355
// , 1436
// , 1522
// , 1614
// , 1711
// , 1813
// , 1922
// , 2038
// , 2161
// , 2291
// , 2429
// , 2575
// , 2731
// , 2896
// , 3071
// , 3257
// , 3454
// , 3664
// , 3886
// , 4123
// , 4374
// , 4640
// , 4924
// , 5225
// , 5545
// , 5886
// , 6248
// , 6633
// , 7042
// , 7478
// , 7942
// , 8437
// , 8963
// , 9524
// , 10122
// , 10759
// , 11439
// , 12165
// , 12940
// , 13767
// , 14652
// , 15598
// , 16610
// , 17694
// , 18855
// , 20100
// , 21436
// , 22870
// , 24413
// , 26072
// , 27860
// , 29788
// , 31869
// , 34119
// , 36555
// , 39197
// , 42066
// , 45188
// , 48593
// , 52312
// , 56386
// , 60860
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
  else if (vcoType == Vco::VCO_TYPE_SAWBENCH)
  {
    inputOffset = 0;//79;
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
