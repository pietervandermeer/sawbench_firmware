#define CLEAR_TRANSIENTS 1

#include <stdint.h>
#include <Envelope.h>
#include <Vco.h>
#include <FrequencyModulator.h>

// one 'tick' : length of 1/64 note in number of PWM periods
#define TICKLEN 900 // 900 FOR SONG, 880 FOR SONG2, 1060 FOR SONG3
#define ADSR_TICKLEN 30

const int midi_in_pin = 2;       // digital pin nr on which you can read the MIDI in signal
const int vco_ms_pwm = 3;           // most significant VCO pwm pin
const int vco_ls_pwm = 5;           // least significant VCO pwm pin
const int saw_vca_pwm = 6;          // envelope control (VCA) pwm pin, for saw signal
const int vco_mode_input_pin = 7;
const int adsr_mode_input_pin = 8;
const int vco_mode_led = 9;
const int adsr_mode_led = 10;
const int square_vca_pwm = 11;          // envelope control (VCA) pvm pin, for square/pwm signal

unsigned short pitch0, pitch, pitch2; // pitch of: lower neighbour note, actual note, higher neighbour note    

#define NOTE_OCTAVE 12

#define NOTE_C0 0
#define NOTE_CS0 1
#define NOTE_D0 2
#define NOTE_DS0 3
#define NOTE_E0 4
#define NOTE_F0 5
#define NOTE_FS0 6
#define NOTE_G0 7
#define NOTE_GS0 8
#define NOTE_A0 9
#define NOTE_AS0 10
#define NOTE_B0 11

#define NOTE_C1 (NOTE_C0 + NOTE_OCTAVE*1)
#define NOTE_CS1 (NOTE_CS0 + NOTE_OCTAVE*1)
#define NOTE_D1 (NOTE_D0 + NOTE_OCTAVE*1)
#define NOTE_DS1 (NOTE_DS0 + NOTE_OCTAVE*1)
#define NOTE_E1 (NOTE_E0 + NOTE_OCTAVE*1)
#define NOTE_F1 (NOTE_F0 + NOTE_OCTAVE*1)
#define NOTE_FS1 (NOTE_FS0 + NOTE_OCTAVE*1)
#define NOTE_G1 (NOTE_G0 + NOTE_OCTAVE*1)
#define NOTE_GS1 (NOTE_GS0 + NOTE_OCTAVE*1)
#define NOTE_A1 (NOTE_A0 + NOTE_OCTAVE*1)
#define NOTE_AS1 (NOTE_AS0 + NOTE_OCTAVE*1)
#define NOTE_B1 (NOTE_B0 + NOTE_OCTAVE*1)

#define NOTE_C2 (NOTE_C0 + NOTE_OCTAVE*2)
#define NOTE_CS2 (NOTE_CS0 + NOTE_OCTAVE*2)
#define NOTE_D2 (NOTE_D0 + NOTE_OCTAVE*2)
#define NOTE_DS2 (NOTE_DS0 + NOTE_OCTAVE*2)
#define NOTE_E2 (NOTE_E0 + NOTE_OCTAVE*2)
#define NOTE_F2 (NOTE_F0 + NOTE_OCTAVE*2)
#define NOTE_FS2 (NOTE_FS0 + NOTE_OCTAVE*2)
#define NOTE_G2 (NOTE_G0 + NOTE_OCTAVE*2)
#define NOTE_GS2 (NOTE_GS0 + NOTE_OCTAVE*2)
#define NOTE_A2 (NOTE_A0 + NOTE_OCTAVE*2)
#define NOTE_AS2 (NOTE_AS0 + NOTE_OCTAVE*2)
#define NOTE_B2 (NOTE_B0 + NOTE_OCTAVE*2)

#define NOTE_C3 (NOTE_C0 + NOTE_OCTAVE*3)
#define NOTE_CS3 (NOTE_CS0 + NOTE_OCTAVE*3)
#define NOTE_D3 (NOTE_D0 + NOTE_OCTAVE*3)
#define NOTE_DS3 (NOTE_DS0 + NOTE_OCTAVE*3)
#define NOTE_E3 (NOTE_E0 + NOTE_OCTAVE*3)
#define NOTE_F3 (NOTE_F0 + NOTE_OCTAVE*3)
#define NOTE_FS3 (NOTE_FS0 + NOTE_OCTAVE*3)
#define NOTE_G3 (NOTE_G0 + NOTE_OCTAVE*3)
#define NOTE_GS3 (NOTE_GS0 + NOTE_OCTAVE*3)
#define NOTE_A3 (NOTE_A0 + NOTE_OCTAVE*3)
#define NOTE_AS3 (NOTE_AS0 + NOTE_OCTAVE*3)
#define NOTE_B3 (NOTE_B0 + NOTE_OCTAVE*3)

#define NOTE_C4 (NOTE_C0 + NOTE_OCTAVE*4)
#define NOTE_CS4 (NOTE_CS0 + NOTE_OCTAVE*4)
#define NOTE_D4 (NOTE_D0 + NOTE_OCTAVE*4)
#define NOTE_DS4 (NOTE_DS0 + NOTE_OCTAVE*4)
#define NOTE_E4 (NOTE_E0 + NOTE_OCTAVE*4)
#define NOTE_F4 (NOTE_F0 + NOTE_OCTAVE*4)
#define NOTE_FS4 (NOTE_FS0 + NOTE_OCTAVE*4)
#define NOTE_G4 (NOTE_G0 + NOTE_OCTAVE*4)
#define NOTE_GS4 (NOTE_GS0 + NOTE_OCTAVE*4)
#define NOTE_A4 (NOTE_A0 + NOTE_OCTAVE*4)
#define NOTE_AS4 (NOTE_AS0 + NOTE_OCTAVE*4)
#define NOTE_B4 (NOTE_B0 + NOTE_OCTAVE*4)

#define NOTE_C5 (NOTE_C0 + NOTE_OCTAVE*5)
#define NOTE_CS5 (NOTE_CS0 + NOTE_OCTAVE*5)
#define NOTE_D5 (NOTE_D0 + NOTE_OCTAVE*5)
#define NOTE_DS5 (NOTE_DS0 + NOTE_OCTAVE*5)
#define NOTE_E5 (NOTE_E0 + NOTE_OCTAVE*5)
#define NOTE_F5 (NOTE_F0 + NOTE_OCTAVE*5)
#define NOTE_FS5 (NOTE_FS0 + NOTE_OCTAVE*5)
#define NOTE_G5 (NOTE_G0 + NOTE_OCTAVE*5)
#define NOTE_GS5 (NOTE_GS0 + NOTE_OCTAVE*5)
#define NOTE_A5 (NOTE_A0 + NOTE_OCTAVE*5)
#define NOTE_AS5 (NOTE_AS0 + NOTE_OCTAVE*5)
#define NOTE_B5 (NOTE_B0 + NOTE_OCTAVE*5)

#define NOTE_C6 (NOTE_C0 + NOTE_OCTAVE*6)
#define NOTE_CS6 (NOTE_CS0 + NOTE_OCTAVE*6)
#define NOTE_D6 (NOTE_D0 + NOTE_OCTAVE*6)
#define NOTE_DS6 (NOTE_DS0 + NOTE_OCTAVE*6)
#define NOTE_E6 (NOTE_E0 + NOTE_OCTAVE*6)
#define NOTE_F6 (NOTE_F0 + NOTE_OCTAVE*6)
#define NOTE_FS6 (NOTE_FS0 + NOTE_OCTAVE*6)
#define NOTE_G6 (NOTE_G0 + NOTE_OCTAVE*6)
#define NOTE_GS6 (NOTE_GS0 + NOTE_OCTAVE*6)
#define NOTE_A6 (NOTE_A0 + NOTE_OCTAVE*6)
#define NOTE_AS6 (NOTE_AS0 + NOTE_OCTAVE*6)
#define NOTE_B6 (NOTE_B0 + NOTE_OCTAVE*6)

#define NOTE_C7 (NOTE_C0 + NOTE_OCTAVE*7)
#define NOTE_CS7 (NOTE_CS0 + NOTE_OCTAVE*7)
#define NOTE_D7 (NOTE_D0 + NOTE_OCTAVE*7)
#define NOTE_DS7 (NOTE_DS0 + NOTE_OCTAVE*7)
#define NOTE_E7 (NOTE_E0 + NOTE_OCTAVE*7)
#define NOTE_F7 (NOTE_F0 + NOTE_OCTAVE*7)
#define NOTE_FS7 (NOTE_FS0 + NOTE_OCTAVE*7)
#define NOTE_G7 (NOTE_G0 + NOTE_OCTAVE*7)
#define NOTE_GS7 (NOTE_GS0 + NOTE_OCTAVE*7)
#define NOTE_A7 (NOTE_A0 + NOTE_OCTAVE*7)
#define NOTE_AS7 (NOTE_AS0 + NOTE_OCTAVE*7)
#define NOTE_B7 (NOTE_B0 + NOTE_OCTAVE*7)

#define NOTE_C8 (NOTE_C0 + NOTE_OCTAVE*8)
#define NOTE_CS8 (NOTE_CS0 + NOTE_OCTAVE*8)
#define NOTE_D8 (NOTE_D0 + NOTE_OCTAVE*8)
#define NOTE_DS8 (NOTE_DS0 + NOTE_OCTAVE*8)
#define NOTE_E8 (NOTE_E0 + NOTE_OCTAVE*8)
#define NOTE_F8 (NOTE_F0 + NOTE_OCTAVE*8)
#define NOTE_FS8 (NOTE_FS0 + NOTE_OCTAVE*8)
#define NOTE_G8 (NOTE_G0 + NOTE_OCTAVE*8)
#define NOTE_GS8 (NOTE_GS0 + NOTE_OCTAVE*8)
#define NOTE_A8 (NOTE_A0 + NOTE_OCTAVE*8)
#define NOTE_AS8 (NOTE_AS0 + NOTE_OCTAVE*8)
#define NOTE_B8 (NOTE_B0 + NOTE_OCTAVE*8)

#define NOTE_C9 (NOTE_C0 + NOTE_OCTAVE*9)
#define NOTE_CS9 (NOTE_CS0 + NOTE_OCTAVE*9)
#define NOTE_D9 (NOTE_D0 + NOTE_OCTAVE*9)
#define NOTE_DS9 (NOTE_DS0 + NOTE_OCTAVE*9)
#define NOTE_E9 (NOTE_E0 + NOTE_OCTAVE*9)
#define NOTE_F9 (NOTE_F0 + NOTE_OCTAVE*9)
#define NOTE_FS9 (NOTE_FS0 + NOTE_OCTAVE*9)
#define NOTE_G9 (NOTE_G0 + NOTE_OCTAVE*9)
#define NOTE_GS9 (NOTE_GS0 + NOTE_OCTAVE*9)
#define NOTE_A9 (NOTE_A0 + NOTE_OCTAVE*9)
#define NOTE_16AS9 (NOTE_AS0 + NOTE_OCTAVE*9)
#define NOTE_B9 (NOTE_B0 + NOTE_OCTAVE*9)

// acid bassline 1?
// note format: pitch.w, interval.w (in TICKLEN), sustain.w (in ADSR_TICKLEN)
unsigned short song_[] =
{
   12*2,4*4,10*8
  ,12*2,4*8,10*8
  ,12*2,4*2,5*8
  ,12*2+2,4*2,5*8
  ,12*2,4*2,5*8
  ,12*2+2,4*2,5*8
  ,12*2,4*2,5*8
  ,12*2+2,4*4,7*8
  ,12*2,4*8,10*8
};

// industrial loop check
// note format: pitch.w, interval.w (in TICKLEN), sustain.w (in ADSR_TICKLEN)
unsigned short song__[] =
{
   12*1,  4*4,5*8
  ,12*1+1,4*1,2*8
  ,12*1+2,4*4,2*8
  ,12*1+1,4*1,2*8
  ,12*1+2,4*4,2*8
};


// 5 quarters?
// note format: pitch.w, interval.w (in TICKLEN), sustain.w (in ADSR_TICKLEN)
unsigned short song___[] =
{
   12*2,  4*4,5*8
  ,12*2+1,4*1,2*8
  ,12*2+2,4*4,2*8
  ,12*2+1,4*1,2*8
  ,12*2+2,4*4,2*8
  ,12*2+1,4*1,2*8
  ,12*2+2,4*3,2*8
  ,12*2+4,4*1,2*8
  ,12*2+3,4*9,4*8
};

// baseline from radioactive man - addict
// note format: pitch.w, interval.w (in TICKLEN), sustain.w (in ADSR_TICKLEN)
unsigned short song[] =
{
   12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*8,5*8

  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8

  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8
  ,12*2+1,  4*2,5*8

  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*4,5*8
  ,12*2-1,  4*8,5*8

  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8
  ,12*2+2,  4*2,5*8

  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
  ,12*2+0,  4*2,5*8
};

// baseline from Elektroids - Midnight Drive
// note format: pitch.w, interval.w (in TICKLEN), sustain.w (in ADSR_TICKLEN)
unsigned short song2[] =
{
   NOTE_C2,  4*4,5*8
  ,NOTE_C2,  4*4,5*8
  ,NOTE_D2,  4*4,5*8
  ,NOTE_E2,  4*4,5*8
  ,NOTE_FS2,  4*4*3,5*8

  ,NOTE_E1,  4*4,8*8
  ,NOTE_C2,  4*12,7*8
  ,NOTE_E1,  4*4,5*8
  ,NOTE_C2,  4*4,5*8
  ,NOTE_E1,  4*4,5*8
  ,NOTE_C2,  4*8,5*8

  ,NOTE_C2,  4*4,5*8
  ,NOTE_C2,  4*4,5*8
  ,NOTE_D2,  4*4,5*8
  ,NOTE_E2,  4*4,5*8
  ,NOTE_FS2,  4*4*3,5*8

  ,NOTE_E1,  4*4,5*8
  ,NOTE_G1,  4*12,7*8
  ,NOTE_E1,  4*4,5*8
  ,NOTE_G1,  4*4,5*8
  ,NOTE_E1,  4*4,5*8
  ,NOTE_G1,  4*8,5*8
};

// baseline from Ceefax Acid Crew - Flogan's Code Part 2
// note format: pitch.w, interval.w (in TICKLEN), sustain.w (in ADSR_TICKLEN)
unsigned short song3[] =
{
// 4*16
   NOTE_C2,  4*4,5*8
  ,NOTE_G1,  4*4,5*8
  ,NOTE_C3,  4*8,8*8
// 4*12
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*4,5*8
  ,NOTE_C3,  4*4,40*8
// 4*32+4*4
  ,NOTE_C1,  4*16+4*4,120*8
  ,NOTE_D1,  4*16,100*8 // TODO: right sustain timing?
  // silence

// 4*16  
  ,NOTE_C2,  4*4,5*8
  ,NOTE_G1,  4*4,5*8
  ,NOTE_C3,  4*8,8*8
// 4*12
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*4,5*8
  ,NOTE_C3,  4*4,40*8
// 4*20
  ,NOTE_C1,  4*12,100*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*4,5*8
// 4*
  ,NOTE_D1,  4*16,150*8 //works, a bit dull
//  ,NOTE_D1,  4*8,40*8 // tricky
//  ,NOTE_C2,  4*2,5*8
//  ,NOTE_C2,  4*2,5*8
//  ,NOTE_C2,  4*2,5*8
//  ,NOTE_C2,  4*4,5*8

  ,NOTE_C2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8

  ,NOTE_C2,  4*4,5*8
  ,NOTE_G1,  4*4,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*4,5*8
  
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8

  ,NOTE_C2,  4*4,5*8
  ,NOTE_G2,  4*4,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*4,5*8
  
};

unsigned short song4[] =
{
   NOTE_G2,  4*16,20*8
  ,NOTE_C2,  4*16,20*8
  ,NOTE_D2,  4*16,20*8
  ,NOTE_G1,  4*16,20*8
  
};

unsigned short song5[] =
{
   NOTE_G2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8

  ,NOTE_C2,  4*4,10*8
  ,NOTE_G1,  4*4,10*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*4,5*8
  
  ,NOTE_D2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_G1,  4*2,5*8

  ,NOTE_G1,  4*4,10*8
  ,NOTE_A1,  4*4,10*8
  ,NOTE_C2,  4*2,5*8
  ,NOTE_C3,  4*2,5*8
  ,NOTE_C2,  4*4,5*8
  
};

/*
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 64: 
      mode = 0x03; 
      break;
    case 256: 
      mode = 0x04; 
      break;
    case 1024: 
      mode = 0x05; 
      break;
    default: 
      return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = (TCCR0B & 0b11111000) | mode;
    } 
    else {
      TCCR1B = (TCCR1B & 0b11111000) | mode;
    }
  } 
  else if(pin == 3 || pin == 11) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 32: 
      mode = 0x03; 
      break;
    case 64: 
      mode = 0x04; 
      break;
    case 128: 
      mode = 0x05; 
      break;
    case 256: 
      mode = 0x06; 
      break;
    case 1024: 
      mode = 0x7; 
      break;
    default: 
      return;
    }
    TCCR2B = (TCCR2B & 0b11111000) | mode;
  }
}

int song_index = 0;
unsigned char note;
int32_t waittime;
int startup = 1;

short amplitude = 0;

unsigned long sustaintime;

int8_t cosLut[] = {127,127,127,127,127,126,126,126,125,124,124,123,122,121,120,119,118,116,115,114,112,111,109,108,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,65,63,60,57,54,51,48,46,43,40,37,34,31,28,24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,-28,-31,-34,-37,-40,-43,-46,-48,-51,-54,-57,-60,-63,-65,-68,-71,-73,-76,-78,-81,-83,-85,-88,-90,-92,-94,-96,-98,-100,-102,-104,-106,-108,-109,-111,-112,-114,-115,-116,-118,-119,-120,-121,-122,-123,-124,-124,-125,-126,-126,-126,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-125,-124,-124,-123,-122,-121,-120,-119,-118,-116,-115,-114,-112,-111,-109,-108,-106,-104,-102,-100,-98,-96,-94,-92,-90,-88,-85,-83,-81,-78,-76,-73,-71,-68,-65,-63,-60,-57,-54,-51,-48,-46,-43,-40,-37,-34,-31,-28,-24,-21,-18,-15,-12,-9,-6,-3,0,3,6,9,12,15,18,21,24,28,31,34,37,40,43,46,48,51,54,57,60,63,65,68,71,73,76,78,81,83,85,88,90,92,94,96,98,100,102,104,106,108,109,111,112,114,115,116,118,119,120,121,122,123,124,124,125,126,126,126,127,127,127,127};
SignalControl::FrequencyModulator fm(&micros, 230, 16000000/ADSR_TICKLEN/64);
SignalControl::Envelope env(&micros, ADSR_TICKLEN*1000 /* adsr ticklen in us */, 64000000UL /* ticks per second */);
Vco vco(vco_ms_pwm, vco_ls_pwm);

void write_vca(uint8_t amplitude)
{
  analogWrite(saw_vca_pwm, amplitude);
}

int fm_div;

// the setup routine runs once when you press reset:
void setup()  {
  Serial.begin(115200);
  Serial.println(".");

  fm.setLut(cosLut);

  // declare pwm pins to be outputs:
  pinMode(saw_vca_pwm, OUTPUT);
  pinMode(square_vca_pwm, OUTPUT);
  pinMode(vco_mode_led, OUTPUT); 
  pinMode(adsr_mode_led, OUTPUT); 
  pinMode(13, OUTPUT);
  // Set pin PWM frequency to 62500 Hz (62500/1 = 62500)
  // Note that the base frequency for pins 5 and 6 is 62500 Hz
  setPwmFrequency(vco_ms_pwm, 1);
  setPwmFrequency(vco_ls_pwm, 1);
  setPwmFrequency(saw_vca_pwm, 1);
  setPwmFrequency(square_vca_pwm, 1);

  // standard piggy vco (saw-only)
  vco.setType(Vco::VCO_TYPE_PIGGY);

  // set sequenced sustain
  

  waittime = TICKLEN*1000;
  startup = 1;
  fm_div=1;
} 

long delta_pitch = 0;
uint32_t lastTickTime;

bool note_triggered = false;
bool note_stopped = false;
int16_t int_amp;

// the loop routine runs over and over again forever:
void loop()
{

#if CLEAR_TRANSIENTS // this crap is sometimes needed to clear transients
  // stay silent during startup period
  if (startup && (waittime >= 0)) {
    analogWrite(vco_ms_pwm, 255);
    analogWrite(vco_ls_pwm, 0);
    delay(ADSR_TICKLEN);
    waittime -= ADSR_TICKLEN;
    return;
  } else {
    startup = 0;
  }
#endif

  // play a song
  if (waittime <= 0) {
    note_triggered = true;
    // new note
    uint16_t note_index = song[song_index++];
    waittime = ((unsigned long) (song[song_index++])) * TICKLEN;
    long sustaintime_raw = song[song_index++];
    if (song_index >= sizeof(song)/sizeof(unsigned short)) {
      song_index = 0;
    }
    // get next note
    unsigned short song_index2 = song_index+3;
    if (song_index2 >= sizeof(song)/sizeof(unsigned short)) {
      song_index2 = 0;
      if (fm_div==1) fm_div=9;
      else if (fm_div==9) fm_div=15;
      else fm_div=1;
    }
    uint16_t note_index2 = song[song_index2];
    
    pitch = vco.getNotePitch(note_index+12*2);

    env.setSustainTime(sustaintime_raw/16);

    fm.setFreq(pitch/fm_div, 16000000/ADSR_TICKLEN/64);
  }

  int32_t fm_amp = (int32_t) fm.step();
  // TODO: modulation depth
  uint16_t modulated_pitch = ((fm_amp * (int32_t)pitch)>>9) + pitch;
  //Serial.println(modulated_pitch);
//  uint16_t modulated_pitch = 0 + pitch;
  vco.write(modulated_pitch);

  // update envelope
  env.Statemachine(note_triggered, note_stopped, int_amp);
  amplitude = int_amp>>6;

  write_vca(amplitude);
//
  uint32_t tickTime;
  do {
    tickTime = micros();
  } while(tickTime - lastTickTime < ADSR_TICKLEN*1000UL);
  lastTickTime = tickTime; 
  
  waittime -= ADSR_TICKLEN;
}

