#include "Statemachine.h"

namespace Midi
{

void Statemachine::note_on_statemachine()
{
  debugln("Note on");
  
  if (new_state)
  {
    note_on_state = note_on_state_key;
    new_state = false;
  }  

  switch (note_on_state)
  {
    case note_on_state_key:
    debugln("note on key");
    note_on_key = midiByte;
    note_on_state = note_on_state_velocity;
    break;

    case note_on_state_velocity:
    {
      debugln("note on velocity");
      uint8_t velocity = midiByte;
      note_on_state = note_on_state_key;

      if (velocity == 0 && active_key == note_on_key)
      {
        // active note is deactivated.
        debugln("note stopped");
        stopped = true;
      }
      else if (velocity)
      {
        // old note is deactivated or new note is activated, we should only handle the latter case!
        debugln("note triggered");
        triggered = true;
        active_velocity = velocity;
        // remember currently playing key
        active_key = note_on_key;
      }
      // else nothing changes
      
    }
    break;
    
    default:
    debugln("Unknown note on state!");
  }
}

void Statemachine::note_off_statemachine()
{
  debugln("Note off");
  
  if (new_state)
  {
    note_off_state = note_off_state_key;
    new_state = false;
  }
  
  switch (note_off_state)
  {
    case note_off_state_key:
    debugln("note off key");
    note_off_key = midiByte;
    note_off_state = note_off_state_velocity;
    break;

    case note_off_state_velocity:
    {
      debugln("note off velocity");
      note_off_velocity = midiByte;
      note_off_state = note_off_state_key;

      // only set volume if the current playing midi should be turned off
      if (active_key == note_off_key)
      {
        stopped = true;
      }
    }
    break;
    
    default:
    debugln("Unknown note on state!");
  }
}

void Statemachine::pitchwheel_statemachine()
{
  debugln("Pitchwheel");
  
  if (new_state)
  {
    pitchwheel_state = pitchwheel_state_lo;
    new_state = false;
  }
    
  switch (pitchwheel_state)
  {
    case pitchwheel_state_lo:
    debugln("low byte");
    pitch_bend = midiByte;
    pitchwheel_state = pitchwheel_state_hi;
    break;

    case pitchwheel_state_hi:
    {
      debugln("high byte");
      pitch_bend |= midiByte<<7;
      pitchwheel_state = pitchwheel_state_lo;
    }
    break;
    
    default:
    debugln("Unknown note on state!");
  }
}

void Statemachine::controlchange_statemachine()
{
  debugln("Control");
  
  if (new_state)
  {
    controlchange_state = controlchange_state_controller;
    new_state = false;
  }
  
  switch (controlchange_state)
  {
    case controlchange_state_controller:
    debugln("controller");
    controlchange_controller = midiByte;
    controlchange_state = controlchange_state_value;
    break;

    case controlchange_state_value:
    {
      debugln("value");
      controlchange_value = midiByte;
      switch (controlchange_controller)
      {
        case controlchange_ctl_modulation_wheel:
          modulation_depth = controlchange_value;
          break;
        case controlchange_ctl_effect1:
          // modulation frequency scalar 127 = .99 , 0 = 0!
          effect1 = controlchange_value;
          break;
        case controlchange_ctl_attack:
          attack_time = controlchange_value;
          break;
        case controlchange_ctl_decay:
          decay_time = controlchange_value;
          break;
        case controlchange_ctl_sustain:
          sustain_level = controlchange_value*2;
          break;
        case controlchange_ctl_release:
          release_time = controlchange_value;
          break;
      }
      controlchange_state = controlchange_state_controller;
    }
    break;
    
    default:
    debugln("Unknown control state!");
  }
}

void Statemachine::programchange_statemachine()
{
  debugln("Program");
  
  if (new_state)
  {
    new_state = false;
  }
  
  program = midiByte;
}

void Statemachine::statemachine() 
{
  // low-level.. get bytes
  uart.decode_runs();
  // read next byte
  short word_ = uart.read_byte();
  if (word_ < 0)
  {
    //debugln(word_);
    return;
  }
  debug("MIDI State = ");
  debughexln(state);
  debug("word_ = ");
  debughexln(word_);
  debug("cmd = ");
  debughexln(word_ & 0xF8);
  if (((unsigned char) word_ & (unsigned char) 0xF8) == 0xF8)
  {
    debugln("Change state!");
    state = (state_t) word_;
    new_state = true;
    return;
  }
  else if ((unsigned char) word_ & 0x80)
  {
    state = (state_t) (word_ & 0xF0);
    rcvd_chan = (unsigned char) word_ & 0x0F;
    new_state = true;
    return;
  }
  // and else.. it's just the usual data byte..
  midiByte = word_;
  
  if (rcvd_chan != chan)
  {
    debugln("Not our midi channel!");
    return;
  }

  switch (state) 
  {
    case state_idle:
    break;
    
    case state_note_on:
    note_on_statemachine();
    break;

    case state_note_off:
    note_off_statemachine();
    break;

    case state_pitchwheel_change:
    pitchwheel_statemachine();
    break;

    case state_controlchange:
    controlchange_statemachine();
    break;
    
    case state_programchange:
    programchange_statemachine();
    break;
    
    default:
    debug("Unhandled MIDI state 0x");
    debughexln(state);
  }
}

void Statemachine::resetTriggerState()
{
  triggered = false;
  stopped = false;
}

} // namespace Midi
