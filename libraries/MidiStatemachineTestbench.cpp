#include <stdio.h>
#include "Statemachine.h"

Midi::Statemachine midi_sm(false);

// TODO: pitchbend, midi effect control, modulation wheel

#define NOTE_C1 24
#define NOTE_D1 25
#define NOTE_C2 36

void print_result(bool res)
{
  if (res)
  {
    printf("success\n");
  }
  else
  {
    printf("FAIL\n");
  }
}

// minimal test of monophonic statemachine, using only note on command
bool test_note_on()
{
  bool res = true;

  // note on: C2, velocity 0x7F
  midi_sm.uart.write_encoded_byte(0x90,1,0);
  midi_sm.uart.write_encoded_byte(NOTE_C2,1,0);
  midi_sm.uart.write_encoded_byte(0x7F,1,1);
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= midi_sm.triggered;
  res &= !midi_sm.stopped;
  res &= (midi_sm.active_key == NOTE_C2);
  res &= (midi_sm.active_velocity == 0x7F);
  printf("C2 on: "); print_result(res);
  midi_sm.resetTriggerState();

  // note on: C1, velocity 0x3F
  midi_sm.uart.write_encoded_byte(0x90,1,0);
  midi_sm.uart.write_encoded_byte(NOTE_C1,1,0);
  midi_sm.uart.write_encoded_byte(0x3F,1,1);
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= midi_sm.triggered;
  res &= !midi_sm.stopped;
  res &= (midi_sm.active_key == NOTE_C1);
  res &= (midi_sm.active_velocity == 0x3F);
  printf("C1 on: "); print_result(res);
  midi_sm.resetTriggerState();

  midi_sm.statemachine();
  res &= !midi_sm.triggered;
  res &= !midi_sm.stopped;
  printf("wait: "); print_result(res);

  // note on: C2, velocity 0x00 (does the same as note off, but doesn't break running status)
  // this should turn off the old note, but should have no effect on the current note (C1)!
  midi_sm.uart.write_encoded_byte(0x90,1,0);
  midi_sm.uart.write_encoded_byte(NOTE_C2,1,0);
  midi_sm.uart.write_encoded_byte(0x00,1,1);
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= !midi_sm.triggered;
  res &= !midi_sm.stopped;
  printf("C2 off (t=%d, s=%d): ", midi_sm.triggered, midi_sm.stopped); print_result(res);
  midi_sm.resetTriggerState();

  // note on: C2, velocity 0x00 (does the same as note off, but doesn't break running status)
  // this should turn off currently playing note!
  midi_sm.uart.write_encoded_byte(0x90,1,0);
  midi_sm.uart.write_encoded_byte(NOTE_C1,1,0);
  midi_sm.uart.write_encoded_byte(0x00,1,1);
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= !midi_sm.triggered;
  res &= midi_sm.stopped;
  printf("C1 off: "); print_result(res);
  midi_sm.resetTriggerState();

  printf("note on : "); print_result(res);
  return res;
}

bool test_attack_time()
{
  bool res = true;
  midi_sm.uart.write_encoded_byte(0xB0,1,0); // control change
  midi_sm.uart.write_encoded_byte(0x49,1,0); // attack time
  midi_sm.uart.write_encoded_byte(0x23,1,1); //
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= (0x23 == midi_sm.attack_time);

  midi_sm.uart.write_encoded_byte(0xB0,1,0); // control change
  midi_sm.uart.write_encoded_byte(0x49,1,0); // attack time
  midi_sm.uart.write_encoded_byte(0x24,1,1); //
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= (0x24 == midi_sm.attack_time);

  printf("attack time : "); print_result(res);
  return res;
}

bool test_release_time()
{
  bool res = true;
  midi_sm.uart.write_encoded_byte(0xB0,1,0); // control change
  midi_sm.uart.write_encoded_byte(0x48,1,0); // release time
  midi_sm.uart.write_encoded_byte(0x23,1,1); //
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= (0x23 == midi_sm.release_time);

  midi_sm.uart.write_encoded_byte(0xB0,1,0); // control change
  midi_sm.uart.write_encoded_byte(0x48,1,0); // release time
  midi_sm.uart.write_encoded_byte(0x24,1,1); //
  // 3 bytes to decode..
  midi_sm.statemachine();
  midi_sm.statemachine();
  midi_sm.statemachine();
  res &= (0x24 == midi_sm.release_time);

  printf("release time : "); print_result(res);
  return res;
}

int main()
{
  bool res = true; // in the beginning everything was fine

  // we expect no notes to be triggered or stopped before any midi data is processed!
  res &= !midi_sm.triggered;
  res &= !midi_sm.stopped;
  if (!res) 
  {
    printf("fail: expected start condition not met\n");
    goto ze_end;
  }

  res &= test_note_on();
  res &= test_attack_time();
  res &= test_release_time();

ze_end:
  printf("MidiStateMachineTestbench : "); print_result(res);
 }
