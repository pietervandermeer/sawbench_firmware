// defining this leaves in all debug statements.. logically, only for debugging ;) if you leave it on it causes serious delays to the main loop!
//#define DEBUG

#ifdef DEBUG
#define debug(args...) Serial.print(args)
#define debugln(args...) Serial.println(args)
#else
#define debug(args...) 
#define debugln(args...) 
#endif

#define CLEAR_TRANSIENTS 1
#define DEMO_MODE 0 // FM modulation depth > 0 at startup

#include <stdint.h>
#include <Statemachine.h>
#include <Envelope.h>
#include <FrequencyModulator.h>
#include <Vco.h>
#include <PiggyLFO.h>

//LFO
//===================================================
unsigned long lfoCounter;
unsigned long lfoSpeed;
int output;
unsigned char state_multiply;
int LED_clock = 0;
LFO LFO1;

//PINS
//===================================================
const int LFOpin = 9;
const int DS_PIN = 8;     //Serial port input 74HC595
const int SHCP_PIN = A4;   //Shift clock input 74HC595
const int STCP_PIN = A5;   //storage clock input 74HC595
const int LFO_BUTTON = 13;
const int POT_BUTTON = 12;

const int POT_4 = A3; //R1 + R2 + ADSR_2_AMOUNT
const int POT_3 = A2; //S1 + S2 + ADSR_1_AMOUNT
const int POT_2 = A1; //D1 + D2 + LFO_AMOUNT
const int POT_1 = A0; //A1 + A2 + LFO_SPEED

#define NUM_OF_LFO_WAVES 4
#define NUM_OF_POT_STATES 3
#define NUM_OF_LEDS 8

//LFO WAVES
//===================================================
unsigned char LFO_state = 0;
unsigned char LFO_buttonState = 0;
unsigned long LFO_buttonCounter = 0;
unsigned char LFO_button_is_on = 0;

//POT STATES
//===================================================
unsigned char POT_state = 0;
unsigned char POT_buttonState = 0;
unsigned long POT_buttonCounter = 0;
unsigned char POT_button_is_on = 0;
  
unsigned long minimum_button_time;
unsigned char registers[8]; 
//===================================================

#define ADSR_TICKLEN 32 // ms/64

const int midi_in_pin = 2;       // digital pin nr on which you can read the MIDI in signal
const int vco_ms_pwm = 3;           // most significant VCO pwm pin
const int vco_ls_pwm = 5;           // least significant VCO pwm pin
const int saw_vca_pwm = 6;          // envelope control (VCA) pwm pin, for saw signal
const int vco_mode_input_pin = 7;

Midi::Statemachine midi_sm(DEMO_MODE == 1 ? true : false);
int32_t fm_amp;
int8_t cosLut[] = {127,127,127,127,127,126,126,126,125,124,124,123,122,121,120,119,118,116,115,114,112,111,109,108,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,65,63,60,57,54,51,48,46,43,40,37,34,31,28,24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,-28,-31,-34,-37,-40,-43,-46,-48,-51,-54,-57,-60,-63,-65,-68,-71,-73,-76,-78,-81,-83,-85,-88,-90,-92,-94,-96,-98,-100,-102,-104,-106,-108,-109,-111,-112,-114,-115,-116,-118,-119,-120,-121,-122,-123,-124,-124,-125,-126,-126,-126,-127,-127,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-125,-124,-124,-123,-122,-121,-120,-119,-118,-116,-115,-114,-112,-111,-109,-108,-106,-104,-102,-100,-98,-96,-94,-92,-90,-88,-85,-83,-81,-78,-76,-73,-71,-68,-65,-63,-60,-57,-54,-51,-48,-46,-43,-40,-37,-34,-31,-28,-24,-21,-18,-15,-12,-9,-6,-3,0,3,6,9,12,15,18,21,24,28,31,34,37,40,43,46,48,51,54,57,60,63,65,68,71,73,76,78,81,83,85,88,90,92,94,96,98,100,102,104,106,108,109,111,112,114,115,116,118,119,120,121,122,123,124,124,125,126,126,126,127,127,127,127};
SignalControl::FrequencyModulator fm(&micros, 230, 16000000/ADSR_TICKLEN/64);
SignalControl::Envelope env(&micros, ADSR_TICKLEN*1000 /* adsr ticklen in us */, 64000000UL /* ticks per second */);
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

inline void write_vca(uint8_t amplitude)
{
  analogWrite(saw_vca_pwm, amplitude);
}

// the setup routine runs once when you press reset:
//===============================================================
void setup()  
{ 
  Serial.begin(115200);
  debugln(".");

  fm.setLut(cosLut);
  
//led driver+lfo wave + pot state
//===================================================
LFO_state = 0;
LFO_buttonState = 0;
LFO_buttonCounter = 0;
LFO_button_is_on = 0;

POT_state = 0;
POT_buttonState = 0;
POT_buttonCounter = 0;
POT_button_is_on = 0;
  
minimum_button_time = 120; //hold time for buttons
  
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

//========================================================
for(int i = 0; i < 8; i++){
    registers[i] = 0;
}
  writeRegister(); //function for writing to shift register

  // declare pwm pins to be outputs:
  pinMode(saw_vca_pwm, OUTPUT);
  //pinMode(13, INPUT);
  // Set pin PWM frequency to 62500 Hz (62500/1 = 62500)
  // Note that the base frequency for pins 5 and 6 is 62500 Hz
  setPwmFrequency(vco_ms_pwm, 1);
  setPwmFrequency(vco_ls_pwm, 1);
  setPwmFrequency(saw_vca_pwm, 1);

  // standard piggy vco (saw-only)
  vco.setType(Vco::VCO_TYPE_PIGGY);

  // silence, i kill u
  write_vca(0);
  
 //LFO 
  pinMode(LFOpin, OUTPUT);
  lfoCounter = 0;
  output = 0;
  setPwmFrequency(LFOpin, 1);
  previousMillis - 0;
  LFO_SPEED = 100;
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

  env.setAttack(midi_sm.attack_time);
  env.setRelease(midi_sm.release_time);
  env.setDecay(midi_sm.decay_time);
  env.setSustainLevel(midi_sm.sustain_level);
  //env.setSustainLevel(100);
  //env.setAttack(5);
  //env.setRelease(60);
}

//--

uint32_t lastTickTime;
uint16_t vca_amp, old_vca_amp;
int16_t int_amp;
uint16_t old_mod_pitch = 0;
uint8_t old_active_key;
uint16_t modulated_pitch;

//Write Register function
//===================================================
void writeRegister()
{
digitalWrite(STCP_PIN, LOW);
  
for (int i = 0; i < 8; i++){
     digitalWrite(SHCP_PIN, LOW);
     digitalWrite(DS_PIN, registers[i]);  
     digitalWrite(SHCP_PIN, HIGH);
  }
  digitalWrite(STCP_PIN, HIGH);  
}
//===================================================

// the loop routine runs over and over again forever:
//===================================================
void loop()
{
  // gpio timing
// digitalWrite(13,1);
  
  // do the actual signal control first in order to eliminate jitter!
  vco.write(modulated_pitch);
  write_vca((vca_amp*synth_velocity)>>8);

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
  }
  old_active_key = midi_sm.active_key;

  synth_set_pitch();

  if (midi_sm.stopped)
    debugln("stopped");
  if (midi_sm.triggered)
    debugln("triggered");

  // update envelope
  env.Statemachine(midi_sm.triggered, midi_sm.stopped, int_amp);
  vca_amp = int_amp >> env.precision;
  if (vca_amp != old_vca_amp)
  {
    debug("vca = "); debugln(vca_amp);
  }
  old_vca_amp = vca_amp;

  // handle modulation
  fm_amp = midi_sm.effect1 ? (int32_t) fm.step() : 0;
  modulated_pitch =  ( ( ((fm_amp * (int32_t)synth_pitch)>>6) * (int32_t)(midi_sm.modulation_depth) ) >>8) + synth_pitch;
#ifdef DEBUG
//  if (modulated_pitch != old_mod_pitch)
//  {
//    debugln(modulated_pitch);
//    debug("fm_amp="); debug(fm_amp); debug(" midi modulation depth="); debugln(midi_sm.modulation_depth);
//    old_mod_pitch = modulated_pitch;
//  }
#endif

  //LFO + KEYTRACK
  //=====================================================
  if(LFO_state == 0){ //random state
    state_multiply = 30;
  }
  else{
    state_multiply = 1;
  }

  // step through
  output = LFO1.LFOout(LFO_state, lfoCounter/256);
  analogWrite(LFOpin, output);//(midi_sm.active_key * 2));
  lfoCounter += lfoSpeed;
  
  //LED DRIVER + LFO WAVE + POT STATE
  //=====================================================
  //LFO PART
  //===================================================
  
  LFO_buttonState = digitalRead(LFO_BUTTON);
  
  if(LFO_buttonState == 1 && LFO_button_is_on == 0){ //LFO button is pushed
    LFO_button_is_on = 1;
    LFO_buttonCounter = 0;
    LFO_buttonCounter = millis();
  }
  if(LFO_buttonState == 0 && (millis() - LFO_buttonCounter) >= minimum_button_time && LFO_button_is_on == 1){ 
    LFO_state = ((LFO_state + 1) % NUM_OF_LFO_WAVES);
    LFO_button_is_on = 0;
    LFO_buttonCounter = 0; 
    }
  if(LFO_buttonState == 0){
    LFO_button_is_on = 0;
    LFO_buttonCounter = 0; 
   }
   
   
  //POT STATES PART
  //===================================================  
  POT_buttonState = digitalRead(POT_BUTTON);
  
  if(POT_buttonState  == 1 && POT_button_is_on == 0) 
  {
    POT_button_is_on = 1;
    POT_buttonCounter = 0;
    POT_buttonCounter = millis();
  }
  if(POT_buttonState == 0 && (millis() - POT_buttonCounter) >= minimum_button_time && POT_button_is_on == 1)
    { 
    POT_state = ((POT_state + 1) % NUM_OF_POT_STATES);
    POT_button_is_on = 0;
    POT_buttonCounter = 0; 
    }
  if(POT_buttonState  == 0)
   {
    POT_button_is_on = 0;
    POT_buttonCounter = 0; 
   }
   
   switch(POT_state)
   {
     case 0: //ADSR 1 
       analogRead(POT_1);
       analogRead(POT_2);
       analogRead(POT_3);
       analogRead(POT_4);
     
     break;
     
     case 1: //ADSR 2
       analogRead(POT_1);
       analogRead(POT_2);
       analogRead(POT_3);
       analogRead(POT_4);
     break;
     
     case 2: //LFO SPEED + LFO AMOUNT + ADSR1 AMOUNT + ADSR2 AMOUNT
       analogRead(POT_1);
       analogRead(POT_2);
       analogRead(POT_3);
       lfoSpeed = (analogRead(POT_4) * state_multiply);
     break;
     
   }
   
  //WRITE REGISTERS TO SHIFT REGISTER
  //===================================================  
  for(int x = 0; x < 7; x++)
  {
    //LFO waves
    //================================================
    for(int x = 0; x < NUM_OF_LFO_WAVES; x++){
       if(x == LFO_state){
        registers[x] = 1;      
       }
       else{
        registers[x] = 0;       
       }
     }
     //POT STATES
     //================================================
     for(int x = 0; x < NUM_OF_POT_STATES; x++){
       if(x == POT_state){
        registers[x+NUM_OF_LFO_WAVES] = 1;      
       }
       else{
        registers[x+NUM_OF_LFO_WAVES] = 0;       
       }
     }
     //LFO clock indicating LED
     //================================================
     if(output > 128)
       registers[7] = 1; //LED on if LFO is high
     else
       registers[7] = 0; //LED on if LFO is high
  
    writeRegister();
  }

  // gpio timing
  //digitalWrite(13,0);

  // 
  // wait until we get a precisely timed loop..
  //

  uint32_t tickTime;
  do {
    tickTime = micros();
  } while(tickTime - lastTickTime < ADSR_TICKLEN*1000UL);
  lastTickTime = tickTime;  

}


