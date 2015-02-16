#ifndef MIDIUART_H
#define MIDIUART_H

#include <Arduino.h>

#define ARDUINO_UNO      0 // UNO, DUEMILLANOVE, ETHERNET, and perhaps even more.. have external interrupt 0 on pin 2
#define ARDUINO_LEONARDO 1 // has external interrupt 1 on pin 1
#define ARDUINO_MEGA     2 // is like the UNO in this respect
#define ARDUINO_DUE      3 // has flexible interrupt per pin

#define ARDUINO_MODEL ARDUINO_UNO
//#define ARDUINO_MODEL ARDUINO_LEONARDO

// external interrupt nr on pin 2 is different for a lot of arduino types

#if ARDUINO_MODEL == ARDUINO_LEONARDO
#define INTERRUPT_NR 1
#endif

#if (ARDUINO_MODEL == ARDUINO_UNO) || (ARDUINO_MODEL == ARDUINO_MEGA) 
#define INTERRUPT_NR 0
#endif

//-------------------------------------------------------------------------------------------
// MIDI stuff
//-------------------------------------------------------------------------------------------

namespace Midi
{
static const unsigned int MIDI_IN_PIN = 2;
static const unsigned int BUF_SIZE = 64; // power of two and max 256!
static const unsigned int RUN_BUF_SIZE = 256; // power of two and max 256!
static const unsigned int MICROS_PER_BIT = (1000000/31250); // exactly 32 usec

class Uart
{
public:
  Uart();
  ~Uart();
  static short read_byte();
  static long read_delta();
  static void decode_runs();
  static void dump_runs();
  // only for test!!
  static void write_bit(unsigned char bit);
  static void write_byte(unsigned char towrite, int n_stop);
  static void write_encoded_byte(unsigned char towrite, int n_stop, unsigned char final);
  static void write_run(unsigned char bit_, unsigned char len);
  static void handle_bitflip_();
  static void handle_bitflip__();

private:
  static volatile unsigned long time2;
  static volatile unsigned char write_idx, read_idx;
  static volatile unsigned char run_write_idx, run_read_idx;
  static volatile unsigned char write_bitnr, read_bitnr;
  static volatile unsigned char buf[BUF_SIZE], run_buf[RUN_BUF_SIZE];

  static void initTimer();
  static int buffer_empty();
  static unsigned char read_bit();
  static unsigned char peek_bits(short bits);
  static short get_bits_available();
  static void attach_interrupt();
};

} // namespace Midi

#endif // MIDIUART_H
