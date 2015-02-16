#include "MidiUart.h"

#ifndef ARDUINO
#include <stdio.h>
#endif

namespace Midi
{
 
// somehow, you need to define static stuff even if it is already declared. .. sigh ..
// oh c++, how i love thee..
volatile unsigned long Uart::time2;
volatile unsigned char Uart::write_idx, Uart::read_idx;
volatile unsigned char Uart::run_write_idx, Uart::run_read_idx;
volatile unsigned char Uart::write_bitnr, Uart::read_bitnr;
volatile unsigned char Uart::buf[BUF_SIZE];
volatile unsigned char Uart::run_buf[RUN_BUF_SIZE];

#define MAX_RUNLEN 10
#define TIMEOUT ((65536*MAX_RUNLEN*MICROS_PER_BIT)/1000000)
// last bit from interrupt land
volatile unsigned char old_bit;
// last bit from (userland) read routine
volatile unsigned char last_bit;

Uart::Uart()
{
#ifndef ARDUINO
  printf("read_idx=%d, write_idx=%d\n", read_idx, write_idx);
  printf("time out counter = %d\n", TIMEOUT); 
#else
  pinMode(MIDI_IN_PIN, INPUT);
#endif
  attach_interrupt();
  time2 = 0; // important for first time of uart interrupt!
  write_idx = 0;
  read_idx = 0;
  write_bitnr = 0;
  read_bitnr = 0;
  run_write_idx=run_read_idx=0;
  old_bit = digitalRead(MIDI_IN_PIN);
  last_bit = 1;
#ifndef ARDUINO
  printf("read_idx=%d, write_idx=%d\n", read_idx, write_idx);
  printf("time out counter = %d\n", TIMEOUT); 
#endif
}

Uart::~Uart()
{
  detachInterrupt(0);
}

// version that keeps in mind timer0 setting (TODO: hardcoded -> actual right implementation using timer0!)
inline unsigned long mmicros()
{
  return (unsigned long) (((long) micros())/64);
}

inline void Uart::attach_interrupt()
{
#if ARDUINO_MODEL == ARDUINO_DUE
  // arduino DUE has flexible pin assignment for the interrupt, hence it expects pin nr instead interrupt nr
  attachInterrupt(MIDI_IN_PIN, handle_bitflip_, CHANGE);
#else
  attachInterrupt(INTERRUPT_NR, handle_bitflip_, CHANGE);
#endif
}

void Uart::initTimer()
{
//  noInterrupts();           // disable all interrupts
  // initialize timer1 
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  TCNT1 = 65536 - TIMEOUT;   // preload timer, for instance: 22 / 65536 = 320 usec
  
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
//  interrupts();             // enable all interrupts
}

#ifdef ARDUINO
ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  //TCNT1 = 0; // disable the timer..
  TIMSK1 = 0;
  Uart::handle_bitflip__();
}
#endif
 
void Uart::handle_bitflip_()
{
  //initTimer();
  handle_bitflip__();
}

inline void Uart::handle_bitflip__()
{
//interrupts();
  // store old bit for next run
  unsigned char bit_ = digitalRead(MIDI_IN_PIN);

  // record timing
  unsigned long time1 = time2;
//noInterrupts();
  time2 = micros();
//interrupts();
  unsigned long delta = (time2 - time1)/64;

  // encode bit run
  //unsigned char len = (delta > MAX_RUNLEN*MICROS_PER_BIT) ? MAX_RUNLEN : (delta+(MICROS_PER_BIT/2))/MICROS_PER_BIT;
  unsigned char len = (delta > MAX_RUNLEN*MICROS_PER_BIT) ? MAX_RUNLEN : (delta+24)/MICROS_PER_BIT;
  run_buf[run_write_idx++] = (len<<1) | old_bit;
  run_write_idx &= RUN_BUF_SIZE-1;
  old_bit = bit_; //digitalRead(MIDI_IN_PIN);
}

const unsigned char revtab[256] =
{
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

static const unsigned char mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void Uart::dump_runs()
{
#ifndef ARDUINO
  printf("read_idx=%d, write_idx=%d\n", read_idx, write_idx);
  for (int i=0; i<BUF_SIZE; i++)
  {
    if (i == read_idx)
    {
      printf("*");
    }
    printf("%02X ", buf[i]);
  }
  printf("\n");
  for (int i=0; i<RUN_BUF_SIZE; i++)
  {
    if (i == run_read_idx)
    {
      printf("*");
    }
    printf("%dx[%d] ", run_buf[i]>>1, run_buf[i]&1);
  }
  printf("\n");
#endif
// TODO: yesduino
}

void Uart::decode_runs()
{
  while (run_read_idx != run_write_idx)
  {
    unsigned char codeword = run_buf[run_read_idx++];
    run_read_idx &= RUN_BUF_SIZE-1;
    unsigned char bit_ = codeword&1;
    unsigned char n_bits = codeword>>1;

#ifdef ARDUINO
//    Serial.print(n_bits);
//    Serial.print("x");
//    Serial.print(bit_);
//    Serial.print(" ");
//Serial.println(codeword);

#endif

    unsigned char i = 0, j, n_bytes = (n_bits+write_bitnr)/8; // assuming n_bits > 0 !
    for (j=0; j <= n_bytes ; j++)
    {
      unsigned char word_ = buf[write_idx];
      for (; i < n_bits && write_bitnr < 8; i++, write_bitnr++) 
      {
        word_ &= ~mask[write_bitnr];
        if (bit_) 
        {
          word_ |= mask[write_bitnr];
        }
      }
      buf[write_idx] = word_;
      if (j < n_bytes)
      {
        write_bitnr &= 7;
        ++write_idx;
        write_idx &= BUF_SIZE-1;
      }
    } // for bytes to write
  } // while runs to do
}

inline unsigned char Uart::read_bit()
{
  unsigned char ret = buf[read_idx] & mask[read_bitnr]; 
  ++read_bitnr;
  if (read_bitnr == 8)
  {
    read_bitnr = 0;
    ++read_idx; 
    read_idx &= BUF_SIZE-1;
  }
  return ret;
}

short Uart::get_bits_available()
{
  short old_read_pos = ((short) read_idx*8) + (short) read_bitnr;
  short write_pos = ((short) write_idx*8) + (short) write_bitnr;
  // linearize wrap-around case
  if (write_pos < old_read_pos)
  {
    write_pos += BUF_SIZE*8;
  }
  // buffer positions are now bitwise and linear so a simple comparison will suffice :)
  return write_pos - old_read_pos;
}

// returns: 0: not enough data, 1: data available
inline unsigned char Uart::peek_bits(short bits)
{
  return get_bits_available() >= bits;
}

inline int Uart::buffer_empty()
{
  return (read_idx == write_idx && read_bitnr == write_bitnr);
}

long Uart::read_delta()
{
  if (run_read_idx != run_write_idx)
  {
    long delta = run_buf[run_read_idx++];
    run_read_idx &= RUN_BUF_SIZE-1;
    return delta;
  }
  return -1;
}

short Uart::read_byte()
{
  unsigned char raw1 = 0;

  // there may be multiple stop bits .. ugh. we read them here because stop bits may be clocked in delayed by the interrupt handler.
  while (1)
  {
    if (last_bit && !(buf[read_idx] & mask[read_bitnr])) // 1->0 (stop->start) transition ?
    {
      break;
    }
    if (buffer_empty())
    {
      return -1;
    }
    last_bit = read_bit();
  } 

  // now read the start bit and data byte. seek ahead to make sure they are available. 
  // otherwise, quit.. next time we start again at the same place (where the start bit should be)
  short bits_avail = get_bits_available();
  if (bits_avail < 9)
  {
    return -1;
  }

  if (read_bit())
  {
    // check start bit (needs to be 0!)
    return -2;
  }
  raw1 = buf[read_idx++] << read_bitnr;
  read_idx &= BUF_SIZE-1;
  raw1 |= buf[read_idx] >> (8 - read_bitnr);

  last_bit = raw1&1;

  // debugging
  unsigned char ret = revtab[raw1];
#ifdef ARDUINO 
#if 0
  if ((ret & 0xF0) == 0x80 || (ret & 0xF0) == 0xA0) // should never occur.. at least not in my debug setup
  { 
    Serial.println(ret,HEX);
    for (int i=0; i<BUF_SIZE; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (int i=0; i<RUN_BUF_SIZE; i++)
    {
      Serial.print(run_buf[i]);
      Serial.print(" ");
    }
    Serial.println();
    Serial.print("read_idx, bitnr = ");
    Serial.print(read_idx);
    Serial.print(", ");
    Serial.println(read_bitnr);
    Serial.print("write_idx, bitnr = ");
    Serial.print(write_idx);
    Serial.print(", ");
    Serial.println(write_bitnr);
    Serial.print("run_read_idx = ");
    Serial.println(run_read_idx);
    Serial.print("run_write_idx = ");
    Serial.println(run_write_idx);
    while(1);
  }
#endif
#endif

  return (unsigned short) ret;
}

// test only

// write a run to run buffer
inline void Uart::write_run(unsigned char bit_, unsigned char len)
{
  run_buf[run_write_idx++] = (len<<1)|bit_;
  run_write_idx &= RUN_BUF_SIZE-1;
}

void Uart::write_encoded_byte(unsigned char towrite, int n_stop, unsigned char final)
{
  static unsigned char old_to_write = 1;
  static int len = 0;
  int i;

  // encode an uart word into a bit array
  unsigned char bufje[10+n_stop];
  bufje[0] = 0; // startbit
  // data bits
  for (i=1; i<=8; i++)
  {
    bufje[i] = (towrite>>(i-1))&1;
  }
  // stop bits
  for (; i<9+n_stop; i++)
  {
    bufje[i] = 1;
  }

  // run length encode the bit array
  for (i = 0; i < 9+n_stop; i++)
  {
    //printf("len=%d, old_to_write=%d\n", len, old_to_write);
    len++;
    if (bufje[i] != old_to_write)
    {
      if (len > 0)
      {
        write_run(old_to_write, len);
      }
      //printf("len=%d\n", len);
      len = 0; 
    }
    old_to_write = bufje[i];
  }
  //printf("len=%d, old_to_write=%d\n", len, old_to_write);
  if (final && (len > 0))
  {
    write_run(old_to_write, len);
  }
}

void Uart::write_bit(unsigned char bit)
{
  buf[write_idx] &= ~(1<<(7-write_bitnr));
  buf[write_idx] |= bit<<(7-write_bitnr);
  ++write_bitnr;
  if (write_bitnr == 8)
  {
    ++write_idx;
    if (write_idx >= BUF_SIZE)
    {
      write_idx = 0;
    }
    write_bitnr = 0;
  }
}

// test only
void Uart::write_byte(unsigned char towrite, int n_stop)
{ 
  write_bit(0); // start
  for (int i=0; i<8; i++)
  {
    write_bit((towrite>>i)&1);
  }
  for (int i=0; i<n_stop; i++)
  {
    write_bit(1); // stop
  }
}

} // namespace Midi
 
