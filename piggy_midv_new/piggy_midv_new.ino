// defining this leaves in all debug statements.. logically, only for debugging ;) if you leave it on it causes serious delays to the main loop!
//#define DEBUG

#ifdef DEBUG
#define debug(args...) Serial.print(args)
#define debugln(args...) Serial.println(args)
#else
#define debug(args...) 
#define debugln(args...) 
#endif

#define BENCHMARK 1 // benchmark loop speed using pin 13..
#define CLEAR_TRANSIENTS 1
#define DEMO_MODE 0 // FM modulation depth > 0 at startup
#define SAWBENCH_CONTROLS 1
#define VCF_CTRL 1

#include <stdint.h>
#include <Statemachine.h>
#include <Envelope.h>
#include <FrequencyModulator.h>
#include <Vco.h>
#include <PiggyLFO.h>
#include <Buttonscroll.h>
#include <Potstates.h>
#include <Writeregisters.h>

//Button + LED's
//===================================================
Writeregister writeregister1;

int potToRead;

Button buttonPOT;
Button buttonLFO;

Potstates POT1;
Potstates POT2;
Potstates POT3;
Potstates POT4;

int Pot1Value;
int Pot2Value;
int Pot3Value;
int Pot4Value;

unsigned char  potMax = 3;
unsigned char  LFOmax = 4;
unsigned char  pot_button_state;
unsigned char  lfo_button_state;
unsigned char  minimum_button_time;
unsigned char pot_index;
unsigned char  lfo_index;

//LFO
//===================================================
unsigned long lfoCounter;
unsigned long lfoSpeed;
unsigned char lfoOutput;
LFO LFO1;

//PINS
//===================================================
const unsigned char LFOpin = 9;
const unsigned char DS_PIN = writeregister1.ds_PIN;     // 8 Serial port input 74HC595
const unsigned char SHCP_PIN = writeregister1.shcp_PIN; //A4 Shift clock input 74HC595
const unsigned char STCP_PIN = writeregister1.stcp_PIN; //A5 storage clock input 74HC595
const unsigned char LFO_BUTTON = 11; // was 13, but wouldn't work with that on-board led connected to it. 
const unsigned char POT_BUTTON = 12;

const unsigned char POT_4 = A3; //R1 + R2 + ADSR_2_AMOUNT
const unsigned char POT_3 = A1; //S1 + S2 + ADSR_1_AMOUNT
const unsigned char POT_2 = A2; //D1 + D2 + LFO_AMOUNT
const unsigned char POT_1 = A0; //A1 + A2 + LFO_SPEED
//===================================================

#define ADSR_TICKLEN 32 // ms/64

const unsigned char midi_in_pin = 2;       // digital pin nr on which you can read the MIDI in signal
const unsigned char vco_ms_pwm = 3;        // most significant VCO pwm pin
const unsigned char vco_ls_pwm = 5;        // least significant VCO pwm pin
const unsigned char saw_vca_pwm = 6;       // envelope control (VCA) pwm pin, for saw signal
const unsigned char vco_mode_input_pin = 7;
const unsigned char vcf_pwm_pin = 10;

Midi::Statemachine midi_sm(DEMO_MODE == 1 ? true : false);
int32_t fm_amp;
int8_t cosLut[] = {127,127,127,127,127,126,126,126,125,124,124,123,122,121,120,119,118,116,115,114,112,111,109,108,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,65,63,60,57,54,51,48,46,43,40,37,34,31,28,24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,-28,-31,-34,-37,-40,-43,-46,-48,-51,-54,-57,-60,-63,-65,-68,-71,-73,-76,-78,-81,-83,-85,-88,-90,-92,-94,-96,-98,-100,-102,-104,-106,-108,-109,-111,-112,-114,-115,-116,-118,-119,-120,-121,-122,-123,-124,-124,-125,-126,-126,-126,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-125,-124,-124,-123,-122,-121,-120,-119,-118,-116,-115,-114,-112,-111,-109,-108,-106,-104,-102,-100,-98,-96,-94,-92,-90,-88,-85,-83,-81,-78,-76,-73,-71,-68,-65,-63,-60,-57,-54,-51,-48,-46,-43,-40,-37,-34,-31,-28,-24,-21,-18,-15,-12,-9,-6,-3,0,3,6,9,12,15,18,21,24,28,31,34,37,40,43,46,48,51,54,57,60,63,65,68,71,73,76,78,81,83,85,88,90,92,94,96,98,100,102,104,106,108,109,111,112,114,115,116,118,119,120,121,122,123,124,124,125,126,126,126,127,127,127,127};
SignalControl::FrequencyModulator fm(&micros, 230, 16000000/ADSR_TICKLEN/64);
SignalControl::Envelope envVca(&micros, ADSR_TICKLEN*1000 /* adsr ticklen in us */, 64000000UL /* ticks per second */);
#if VCF_CTRL
SignalControl::Envelope envVcf(&micros, ADSR_TICKLEN*1000 /* adsr ticklen in us */, 64000000UL /* ticks per second */);
#endif
Vco vco(vco_ms_pwm, vco_ls_pwm);

//-------------------------------------------------------------------------------------------
// electronics control
//-------------------------------------------------------------------------------------------

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

// the setup routine runs once when you press reset:
//===============================================================
void setup()  
{ 
  Serial.begin(115200);
  debugln(".");

  fm.setLut(cosLut);
  
  pinMode(DS_PIN, OUTPUT);
  pinMode(SHCP_PIN, OUTPUT);
  pinMode(STCP_PIN, OUTPUT);

  pinMode(LFO_BUTTON, INPUT_PULLUP);
  pinMode(POT_BUTTON, INPUT_PULLUP);

  //POTS
  //========================================================
  pinMode(POT_4, INPUT);
  pinMode(POT_3, INPUT);
  pinMode(POT_2, INPUT);
  pinMode(POT_1, INPUT);

  // declare pwm pins to be outputs:
  pinMode(saw_vca_pwm, OUTPUT);
#if VCF_CTRL
  pinMode(vcf_pwm_pin, OUTPUT);
  setPwmFrequency(vcf_pwm_pin, 1);
#endif
  //pinMode(13, INPUT);
  // Set pin PWM frequency to 62500 Hz (62500/1 = 62500)
  // Note that the base frequency for pins 5 and 6 is 62500 Hz
  setPwmFrequency(vco_ms_pwm, 1);
  setPwmFrequency(vco_ls_pwm, 1);
  setPwmFrequency(saw_vca_pwm, 1);

  // standard piggy vco (saw-only)
  vco.setType(Vco::VCO_TYPE_PIGGY);

  // silence, i kill u
  analogWrite(saw_vca_pwm, 0);
  
  //LFO 
  pinMode(LFOpin, OUTPUT);
  lfoCounter = 0;
  lfoOutput = 0;
  setPwmFrequency(LFOpin, 1);
  lfoSpeed = 100;

  // reading digitized potmeters round-robin
  potToRead = 0;
  
  pinMode(13, OUTPUT);
} 

//--

// actual synth engine variables 
uint16_t pitch = 0, old_pitch, synth_pitch;
uint16_t synth_velocity;

void synth_set_pitch()
{
  long pitch_bend; //fixedpoint
  pitch_bend = (long) (midi_sm.pitch_bend); // - 8192);
  pitch_bend *= pitch;
  pitch_bend >>= 16;
  synth_pitch = pitch + pitch_bend;

  // directly set up the pitch and volume
  fm.setFreq(( (uint32_t) synth_pitch * (uint32_t) midi_sm.effect1 )>>7, 16000000/ADSR_TICKLEN/64);
#if 0
  // TODO: can be overridden by onboard controls
  envVca.setAttack(midi_sm.attack_time);
  envVca.setRelease(midi_sm.release_time);
  envVca.setDecay(midi_sm.decay_time);
  envVca.setSustainLevel(midi_sm.sustain_level);
  //envVca.setSustainLevel(100);
  //envVca.setAttack(5);
  //envVca.setRelease(60);
#if VCF_CTRL
  // TODO: separate midi adsr for vcf..
  envVcf.setAttack(midi_sm.attack_time);
  envVcf.setRelease(midi_sm.release_time);
  envVcf.setDecay(midi_sm.decay_time);
  envVcf.setSustainLevel(midi_sm.sustain_level);
#endif
#endif
}

//--

uint32_t lastTickTime;
uint16_t vca_amp, old_vca_amp;

uint16_t vcf_amp, old_vcf_amp;
bool vcf_stop;

uint16_t old_mod_pitch = 0;
uint8_t old_active_key;
uint16_t modulated_pitch;

// the loop routine runs over and over again forever:
//===================================================
void loop()
{
  int16_t int_amp;

#if BENCHMARK
  // gpio timing
  digitalWrite(13,1);
#endif

  // do the actual signal control first in order to eliminate jitter!
  vco.write(modulated_pitch);
  analogWrite(saw_vca_pwm, (vca_amp*synth_velocity)>>8);
#if VCF_CTRL
  analogWrite(vcf_pwm_pin, (vcf_amp*synth_velocity)>>8);
#endif

  // handle midi state machine
  midi_sm.statemachine();

  // process midi state machine variables to synth variables
  // TODO: callbacks may be better?
  synth_velocity = ((uint16_t) (midi_sm.active_velocity))<<2;
  // volume clipping
  if (synth_velocity > 255)
  {
    synth_velocity = 255;
  }
  pitch = vco.getNotePitch(midi_sm.active_key);
  if (old_active_key != midi_sm.active_key)
  {
    debugln("active key!");
    debugln(midi_sm.active_key);
  }
  old_active_key = midi_sm.active_key;

  synth_set_pitch();

  if (midi_sm.stopped)
    debugln("stopped");
  if (midi_sm.triggered)
    debugln("triggered");

  bool vcf_trig = midi_sm.triggered;
  if (midi_sm.stopped)
  {
    vcf_stop = true;
  }
  
  // update VCA envelope
  envVca.Statemachine(midi_sm.triggered, midi_sm.stopped, int_amp);
  vca_amp = int_amp >> envVca.precision;
  if (vca_amp != old_vca_amp)
  {
    debug("vca = "); debugln(vca_amp);
  }
  old_vca_amp = vca_amp;
#if VCF_CTRL
  // update VCF envelope
  envVcf.Statemachine(vcf_trig, vcf_stop, int_amp);
  vcf_amp = int_amp >> envVcf.precision;
  if (vcf_amp != old_vcf_amp)
  {
    debug("vcf = "); debugln(vcf_amp);
  }
  old_vcf_amp = vcf_amp;
#endif
  // handle modulation
  fm_amp = midi_sm.effect1 ? (int32_t) fm.step() : 0;
  modulated_pitch =  ( ( ((fm_amp * (int32_t)synth_pitch)>>6) * (int32_t)(midi_sm.modulation_depth) ) >>8) + synth_pitch;
#ifdef DEBUG
  if (modulated_pitch != old_mod_pitch)
  {
    debugln(modulated_pitch);
    debug("fm_amp="); debug(fm_amp); debug(" midi modulation depth="); debugln(midi_sm.modulation_depth);
    old_mod_pitch = modulated_pitch;
  }
#endif
  //BUTTON + LED'S
  //=====================================================

  // round robin which pot to read, because analogRead() is very slow.. and we don't need to read out at 2 kHz.. 60 Hz is already ok.
  switch(potToRead)
  {
    case 0:
    Pot1Value = analogRead(POT_1);
    break;
    case 1:
    Pot2Value = analogRead(POT_2);
    break;
    case 2:
    Pot3Value = analogRead(POT_3);
    break;
    case 3:
    Pot4Value = analogRead(POT_4); 
    break;
  }
  
  potToRead++; 
  if (potToRead > 3)
  {
    potToRead = 0;
  }
    
  pot_button_state = digitalRead(POT_BUTTON);
  lfo_button_state = digitalRead(LFO_BUTTON);
  
  pot_index = buttonPOT.ButtonScroll(potMax, pot_button_state, minimum_button_time);
  lfo_index = buttonLFO.ButtonScroll(LFOmax, lfo_button_state, minimum_button_time);
  
  writeregister1.LedWriter( 4, lfo_index, pot_index);
  
  switch(pot_index)
  {
    case 0:                                                                 //VCA         
    envVca.setAttack(POT1.potStatesWrite(pot_index, Pot1Value) / 8);        //Attack
    envVca.setDecay(POT2.potStatesWrite(pot_index, Pot2Value) / 8);         //DECAY
    envVca.setSustainLevel(POT3.potStatesWrite(pot_index, Pot3Value) / 4);  //SUSTAIN
    envVca.setRelease(POT4.potStatesWrite(pot_index, Pot4Value) / 8);       //RELEASE
    break;
    
    case 1:                                                                 //VCF
    envVcf.setAttack(POT1.potStatesWrite(pot_index, Pot1Value) / 8);        //Attack
    envVcf.setDecay(POT2.potStatesWrite(pot_index, Pot2Value) / 8);         //DECAY
    envVcf.setSustainLevel(POT3.potStatesWrite(pot_index, Pot3Value) / 4);  //SUSTAIN
    envVcf.setRelease(POT4.potStatesWrite(pot_index, Pot4Value) / 8);       //RELEASE
    break;
    
    case 2:
    lfoSpeed = POT1.potStatesWrite(pot_index, Pot1Value);  //LFO_SPEED
    if (lfoSpeed < 8)
    {
      lfoSpeed = 8;
    }
    POT2.potStatesWrite(pot_index, Pot2Value);  //LFO AMOUNT
    POT3.potStatesWrite(pot_index, Pot3Value);  //ADSR1 AMOUNT
    POT4.potStatesWrite(pot_index, Pot4Value);  //ADSR2 AMOUNT
    break;
  }

  // step through
  lfoOutput = LFO1.LFOout(lfo_index, lfoCounter/256);
  analogWrite(LFOpin, lfoOutput);
  lfoCounter += lfoSpeed;
  //=====================================================

#ifdef BENCHMARK
  // gpio timing
  digitalWrite(13,0);
#endif

  // 
  // wait until we get a precisely timed loop..
  //

  uint32_t tickTime;
  do {
    tickTime = micros();
  } while(tickTime - lastTickTime < ADSR_TICKLEN*1000UL);
  lastTickTime = tickTime;  

}

