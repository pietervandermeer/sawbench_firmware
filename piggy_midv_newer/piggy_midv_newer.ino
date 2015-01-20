// defining this leaves in all debug statements.. logically, only for debugging ;) if you leave it on it causes serious delays to the main loop!
//#define DEBUG

#ifdef DEBUG
#define debug(args...) Serial.print(args)
#define debugln(args...) Serial.println(args)
#else
#define debug(args...) 
#define debugln(args...) 
#endif

#define BENCHMARK 0 // benchmark loop timing using pin 13..
#define CLEAR_TRANSIENTS 1
#define SAWBENCH_CONTROLS 1

#include <stdint.h>
#include <Statemachine.h>
#include <SimpleEnvelope.h>
#include <FrequencyModulator.h>
#include <Vco.h>
#include <PiggyLFO.h>
#include <Buttonscroll.h>
#include <Potstates.h>
#include <Writeregisters.h>

//Button + LED's
//===================================================
Writeregister writeregister1;

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

#define ADSR_TICKLEN 64 // ms/64

const unsigned char midi_in_pin = 2;       // digital pin nr on which you can read the MIDI in signal
const unsigned char vco_ms_pwm = 3;        // most significant VCO pwm pin
const unsigned char vco_ls_pwm = 5;        // least significant VCO pwm pin
const unsigned char saw_vca_pwm = 6;       // envelope control (VCA) pwm pin, for saw signal
const unsigned char vco_mode_input_pin = 7;
const unsigned char vcf_pwm_pin = 10;

Midi::Statemachine midi_sm;
int32_t fm_amp;
int8_t cosLut[] = {127,127,127,127,127,126,126,126,125,124,124,123,122,121,120,119,118,116,115,114,112,111,109,108,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,65,63,60,57,54,51,48,46,43,40,37,34,31,28,24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,-28,-31,-34,-37,-40,-43,-46,-48,-51,-54,-57,-60,-63,-65,-68,-71,-73,-76,-78,-81,-83,-85,-88,-90,-92,-94,-96,-98,-100,-102,-104,-106,-108,-109,-111,-112,-114,-115,-116,-118,-119,-120,-121,-122,-123,-124,-124,-125,-126,-126,-126,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-125,-124,-124,-123,-122,-121,-120,-119,-118,-116,-115,-114,-112,-111,-109,-108,-106,-104,-102,-100,-98,-96,-94,-92,-90,-88,-85,-83,-81,-78,-76,-73,-71,-68,-65,-63,-60,-57,-54,-51,-48,-46,-43,-40,-37,-34,-31,-28,-24,-21,-18,-15,-12,-9,-6,-3,0,3,6,9,12,15,18,21,24,28,31,34,37,40,43,46,48,51,54,57,60,63,65,68,71,73,76,78,81,83,85,88,90,92,94,96,98,100,102,104,106,108,109,111,112,114,115,116,118,119,120,121,122,123,124,124,125,126,126,126,127,127,127,127};
SignalControl::FrequencyModulator fm(&micros, 230, 16000000/ADSR_TICKLEN/64);
SimpleEnvelope envVca;
SimpleEnvelope envVcf;
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

void setVcaAttack(int v)
{
  envVca.setAttack(v/8);
}

void setVcaRelease(int v)
{
  envVca.setRelease(v/8);
}

void setVcaDecay(int v)
{
  envVca.setDecay(v/8);
}

void setVcaSustainLevel(int v)
{
  envVca.setSustainLevel(v/4);
}

void setVcfAttack(int v)
{
  envVcf.setAttack(v/8);
}

void setVcfRelease(int v)
{
  envVcf.setRelease(v/8);
}

void setVcfDecay(int v)
{
  envVcf.setDecay(v/8);
}

void setVcfSustainLevel(int v)
{
  envVcf.setSustainLevel(v/4);
}

void setLfoSpeed(int v)
{
  lfoSpeed = v;
  if (lfoSpeed < 8)
  {
    lfoSpeed = 8;
  }
}

uint8_t lfoAmount = 0;

void setLfoAmount(int v)
{
  lfoAmount = v/4;
}

uint8_t adsr1Amount=128, adsr2Amount = 128;

void setAdsr1Amount(int v)
{
  adsr1Amount = v/4;
}

void setAdsr2Amount(int v)
{
  adsr2Amount = v/4;
}

typedef enum
{
 controlchange_ctl_bankchange=0
,controlchange_ctl_modulation_wheel=1
,controlchange_ctl_breath=2
,controlchange_ctl_undef=3
,controlchange_ctl_foot=4
,controlchange_ctl_effect1=0x0C
,controlchange_ctl_effect2=0x0D
,controlchange_ctl_sustainpedal=0x40
,controlchange_ctl_resonance=0x47
,controlchange_ctl_release=0x48 // 72
,controlchange_ctl_attack=0x49 // 73
,controlchange_ctl_brightness=0x4A
,controlchange_ctl_decay=0x4B // 75
,controlchange_ctl_sustain=0x50 // 80
,controlchange_ctl_vcf_release=0x54
,controlchange_ctl_vcf_attack=0x51
,controlchange_ctl_vcf_decay=0x52
,controlchange_ctl_vcf_sustain=0x53
,controlchange_ctl_lfo_speed=0x55
,controlchange_ctl_lfo_amount=0x56
,controlchange_ctl_adsr1_amount=0x57
,controlchange_ctl_adsr2_amount=0x58
,controlchange_ctl_vcf_tracking=0x59 // 89
// TODO: more!
} controlchange_controller_t;


uint8_t modulation_depth, effect1 = 27, vcfTracking = 64;

void midi_cc_callback(uint8_t cc, uint8_t val)
{
  debug("cc callback "); debug(cc); debugln(val);
  switch ((controlchange_controller_t) cc)
  {
    case controlchange_ctl_modulation_wheel:
      modulation_depth = val;
      break;
    case controlchange_ctl_effect1:
      // modulation frequency scalar 127 = .99 , 0 = 0!
      effect1 = val;
      break;
    case controlchange_ctl_brightness:
      // TODO: overwrite
      break;
    case controlchange_ctl_attack:
      envVca.setAttack(val);
      break;
    case controlchange_ctl_decay:
      envVca.setDecay(val);
      break;
    case controlchange_ctl_sustain:
      envVca.setSustainLevel(val*2);
      break;
    case controlchange_ctl_release:
      envVca.setRelease(val);
      break;
    case controlchange_ctl_vcf_attack:
      envVcf.setAttack(val);
      break;
    case controlchange_ctl_vcf_decay:
      envVcf.setDecay(val);
      break;
    case controlchange_ctl_vcf_sustain:
      envVcf.setSustainLevel(val*2);
      break;
    case controlchange_ctl_vcf_release:
      envVcf.setRelease(val);
      break;
    case controlchange_ctl_lfo_speed:
      lfoSpeed = val*2;
      break;
    case controlchange_ctl_lfo_amount:
      lfoAmount = val*2;
      break;
    case controlchange_ctl_adsr1_amount:
      adsr1Amount = val*2;
      break;
    case controlchange_ctl_adsr2_amount:
      adsr2Amount = val*2;
      break;
    case controlchange_ctl_vcf_tracking:
      vcfTracking = val;
      break;
    default:
      break;
  }
}

int16_t lfoLedError;
bool lfoLed;

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
  pinMode(vcf_pwm_pin, OUTPUT);
  setPwmFrequency(vcf_pwm_pin, 1);
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

  midi_sm.register_cc_callback(midi_cc_callback);
  
  POT1.registerCallback(0, setVcaAttack);
  POT1.registerCallback(1, setVcfAttack);
  POT1.registerCallback(2, setLfoSpeed);
  POT2.registerCallback(0, setVcaDecay);
  POT2.registerCallback(1, setVcfDecay);
  POT2.registerCallback(2, setLfoAmount);
  POT3.registerCallback(0, setVcaSustainLevel);
  POT3.registerCallback(1, setVcfSustainLevel);
  POT3.registerCallback(2, setAdsr1Amount);
  POT4.registerCallback(0, setVcaRelease);
  POT4.registerCallback(1, setVcfRelease);
  POT4.registerCallback(2, setAdsr2Amount);

  lfoLedError = 0;
  
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
  fm.setFreq(( (uint32_t) synth_pitch * (uint32_t) effect1 )>>7, 16000000/ADSR_TICKLEN/64);
}

//--

uint32_t lastTickTime;
uint16_t vca_amp, old_vca_amp;
uint16_t vcf_amp, old_vcf_amp;

uint16_t old_mod_pitch = 0;
uint8_t old_active_key;
uint16_t modulated_pitch;

uint8_t loopCount = 0;

// the loop routine runs over and over again forever:
//===================================================
void loop()
{
#if BENCHMARK
  // gpio timing
  digitalWrite(13,1);
#endif

  // do the actual signal control first in order to eliminate jitter!
  vco.write(modulated_pitch);
  analogWrite(saw_vca_pwm, (vca_amp*synth_velocity)>>8);
  analogWrite(vcf_pwm_pin, vcf_amp + ((midi_sm.active_key * (uint16_t) vcfTracking)/64) );

  // handle midi state machine
  midi_sm.statemachine();

  // a = (m*v+b)^2, v: midi velocity, m = 1-b, b = 126/(127*sqrt(range))
  synth_velocity = midi_sm.active_velocity+0x68;
  synth_velocity *= synth_velocity;
  synth_velocity = ((uint32_t) synth_velocity * (uint32_t) adsr1Amount) / 8192;

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

  // MIDI note on / note off -> envelope control
  if (midi_sm.triggered)
  {
    debugln("triggered");
    envVca.trigger();
    envVcf.trigger();
    midi_sm.triggered = false;
  }
  if (midi_sm.stopped)
  {
    debugln("stopped");
    envVca.stop();
    envVcf.stop();
    midi_sm.stopped = false;
  }
  //
  // update envelopes
  //

  if (loopCount&1)
  {  
    envVca.run();
    vca_amp = envVca.output >> envVca.precision;
    if (vca_amp != old_vca_amp)
    {
      debug("vca = "); debugln((long)envVca.output);
      debug("vca = "); debugln(vca_amp);
    }
    old_vca_amp = vca_amp;
  }
  else
  {
    // update VCF envelope
    envVcf.run();
    vcf_amp = ((envVcf.output >> envVcf.precision) * adsr2Amount) /256;
    if (vcf_amp != old_vcf_amp)
    {
      debug("vcf = "); debugln(vcf_amp);
    }
    old_vcf_amp = vcf_amp;
  }

  // handle modulation
  fm_amp = effect1 ? (int32_t) fm.step() : 0;
  modulated_pitch =  ( ( ((fm_amp * (int32_t)synth_pitch)>>6) * (int32_t)(modulation_depth) ) >>8) + synth_pitch;
#ifdef DEBUG
  if (modulated_pitch != old_mod_pitch)
  {
    debugln(modulated_pitch);
    debug("fm_amp="); debug(fm_amp); debug(" midi modulation depth="); debugln(modulation_depth);
    old_mod_pitch = modulated_pitch;
  }
#endif
  //BUTTON + LED'S
  //=====================================================

  // round robin which pot to read, because analogRead() is very slow.. and we don't need to read out at 2 kHz.. 60 Hz is already ok.
  switch(loopCount%4)
  {
    case 0:
    Pot1Value = analogRead(POT_1);
    POT1.potStatesWrite(pot_index, Pot1Value);
    break;
    case 1:
    Pot2Value = analogRead(POT_2);
    POT2.potStatesWrite(pot_index, Pot2Value);
    break;
    case 2:
    POT3.potStatesWrite(pot_index, Pot3Value);
    Pot3Value = analogRead(POT_3);
    break;
    case 3:
    POT4.potStatesWrite(pot_index, Pot4Value);
    Pot4Value = analogRead(POT_4); 
    break;
  }
  
  pot_button_state = digitalRead(POT_BUTTON);
  lfo_button_state = digitalRead(LFO_BUTTON);
  
  pot_index = buttonPOT.ButtonScroll(potMax, pot_button_state, minimum_button_time);
  lfo_index = buttonLFO.ButtonScroll(LFOmax, lfo_button_state, minimum_button_time);
  
  lfoLedError += lfoOutput - (lfoLed*256);
  lfoLed = (lfoLedError >= 128);
  writeregister1.LedWriter(4, lfo_index, pot_index, lfoLed);

  // step through
  lfoOutput = LFO1.LFOout(lfo_index, lfoCounter/256);
  analogWrite(LFOpin, ((uint16_t) lfoOutput * (uint16_t) lfoAmount)/256);
  lfoCounter += lfoSpeed;
  //=====================================================

  loopCount++;

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

