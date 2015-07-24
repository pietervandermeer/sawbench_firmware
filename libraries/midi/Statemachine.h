// MIDI statemachine for monophonic synthesizer (tracks just one active note)
//#define DEBUG

#ifndef MIDI_STATEMACHINE_H
#define MIDI_STATEMACHINE_H

#include <stdint.h>
#include <Arduino.h>
#include "MidiUart.h"

#ifdef DEBUG 

#ifndef debug

#ifdef ARDUINO
#define debug(args...) Serial.print(args)
#define debugln(args...) Serial.println(args)
#define debughexln(arg) Serial.println(arg, HEX)
#else // ARDUINO
#include <stdio.h>
void debug(const char* str) 
{
	printf("%s", str);
}
void debugln(const char* str) 
{
	printf("%s\n", str);
}
void debug(int i) 
{
	printf("%d", i);
}
void debugln(int i) 
{
	printf("%d\n", i);
}
void debughexln(int hex) 
{
	printf("%X\n", hex);
}
#endif // ARDUINO 

#endif // debug

#else // DEBUG

#define debug(args...) 
#define debugln(args...) 
#define debughexln(args...) 

#endif

namespace Midi
{

class Statemachine
{
public:
	Statemachine() 
	{
		// TODO: all of this should be transferred to an observer.
		chan = 0;
		// set initial state
		//state_t state = state_idle;
		state = state_note_on; //useful for the ht-700..
		active_key = -1; // non-existing midi key.
		// no note event initially
		triggered = false;
		stopped = false;
		//effect1 = 0; //FM=off
		// pitch bend centered
		pitch_bend = 8192;
		// disable CC observation callback by default. 
		cc_callback = 0;
	}

	~Statemachine() {}

	void register_cc_callback( void (*callback)(uint8_t, uint8_t) );
	void note_on_statemachine();
	void note_off_statemachine();
	void pitchwheel_statemachine();
	void controlchange_statemachine();
	void programchange_statemachine();
	void run();
	void resetTriggerState();
	void setLegatoMode(bool mode);

	// public member, up for grabs. could be done with getters since they're read-only, but well.. maybe when i have time!

	// the midi channel the piggy obeys..
	uint8_t chan;
	bool new_state;
	uint8_t midiByte;
	uint8_t active_key;
	uint8_t note_on_key; 
	uint8_t active_velocity;
	uint8_t controlchange_controller, controlchange_value; 
	bool triggered, stopped;
	uint16_t pitch_bend;
	uint8_t program;
	uint8_t note_off_key, note_off_velocity;
	uint8_t notesActive;
	bool legatoMode;

	void (*cc_callback)(uint8_t, uint8_t);

	Uart uart;

private:
	uint8_t rcvd_chan;

	// all statemachine states should be private
	typedef enum
	{
	  state_idle                = 0    //0
	 ,state_note_off            = 0x80 //1000....
	 ,state_note_on             = 0x90 //1001....
	 ,state_polykey_pressure    = 0xA0 //1010....
	 ,state_controlchange       = 0xB0 //1011....
	 ,state_programchange       = 0xC0 //1100....
	 ,state_channel_pressure    = 0xD0 //1101....
	 ,state_pitchwheel_change   = 0xE0 //1110....
	 ,state_system_exclusive    = 0xF0 //11110000
	 ,state_quarter_frame       = 0xF1 //11110001
	 ,state_songpos             = 0xF2 //11110010
	 ,state_songselect          = 0xF3 //11110011
	 ,state_tunerequest         = 0xF6 //11110110
	 ,state_end_exclusive       = 0xF7 //11110111
	 ,state_timing_clock        = 0xF8 //11111000
	 ,state_start               = 0xFA //11111010
	 ,state_continue            = 0xFB //11111011
	 ,state_stop                = 0xFC //11111100
	 ,state_active_sensing      = 0xFE //11111110
	 ,state_reset               = 0xFF //11111111
	} state_t;

	state_t state;

	typedef enum
	{
	  note_on_state_key
	 ,note_on_state_velocity
	} note_on_state_t;

	note_on_state_t note_on_state;

	typedef enum
	{
	  note_off_state_key
	 ,note_off_state_velocity
	} note_off_state_t;

	note_off_state_t note_off_state;

	typedef enum
	{
	  programchange_state_program
	} programchange_state_t;

	programchange_state_t programchange_state;

	typedef enum
	{
	  pitchwheel_state_lo
	 ,pitchwheel_state_hi
	} pitchwheel_state_t;

	typedef enum
	{
	  controlchange_state_controller
	 ,controlchange_state_value
	} controlchange_state_t;

	pitchwheel_state_t pitchwheel_state;
	controlchange_state_t controlchange_state;

};

} // namespace Midi

#endif //MIDI_STATEMACHINE_H
