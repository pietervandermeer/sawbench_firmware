#include <stdio.h>
#include "MidiUart.h"

Midi::Uart uart;

void printread()
{
  short sh = -1;
  sh = uart.read_byte();
  if (sh >= 0) 
  {
    printf("%02X\n", sh);
  }
  else
  {
    printf("%d\n", sh);
  }
}

void test_partial_writes()
{
  for (int i=0; i<1; i++)
  {
    uart.write_byte(0xDE,1);
    printread();
    printread();
    uart.write_byte(0xAD,2);
    printread();
    printread();
    uart.write_byte(0xBE,2);
//    uart.write_byte(0xBE,2);
//    uart.write_byte(0xBE,2);
    printread();
    printread();
//    uart.write_byte(0xEF,1);
    printread();
    uart.write_bit(1); //stop bit
    uart.write_bit(0); // start bit
    uart.write_bit(1);
    uart.write_bit(0);
    uart.write_bit(1);
    uart.write_bit(0);
    uart.write_bit(1);
    uart.write_bit(0);
    uart.write_bit(1);
    printread(); // should give -1..
    uart.write_bit(0);
    printread(); // should give 0x55
    uart.write_bit(1); //stop bit
    uart.write_bit(1); //stop bit
    uart.write_bit(1); //stop bit
//    printread();
//    printread();
  }
}

void test_entire_writes()
{
  uart.write_byte(0xDE,1);
  uart.write_byte(0xAD,2);
  uart.write_byte(0xBE,1);
  uart.write_byte(0xEF,1);
  for (int i=0; i<4; i++)
  {
    printread();
  }
}

int checkread(int wanted)
{
  int got = uart.read_byte();
  int ret = wanted == got;
  if (!ret) printf("fail [wanted=%02X, got=%02X]!\n", wanted, got);
  return ret;
}

#define NUM_REPEATS 1

void test_rle()
{
  int ret = 1;
  printf("==RLE==\n");
  for (int j = NUM_REPEATS; j > 0; j--)
  {
    uart.write_encoded_byte(0xDE,1,0);
    uart.dump_runs();
    uart.write_encoded_byte(0xAD,2,0);
    uart.dump_runs();
    uart.write_encoded_byte(0xBE,1,0);
    uart.dump_runs();

    uart.decode_runs();
    ret &= checkread(0xDE);
    ret &= checkread(0xAD);

    uart.write_encoded_byte(0x13,1,0);
    uart.dump_runs();
    uart.write_encoded_byte(0x3E,2,0);
    uart.dump_runs();

    uart.decode_runs();
    ret &= checkread(0xBE);
    ret &= checkread(0x13);

    uart.write_encoded_byte(0x3E,1,0);
    uart.dump_runs();
    uart.write_encoded_byte(0xEF,1,1);
    uart.dump_runs();
    uart.write_run(1,1); // one more stop bit to finish it off..
    uart.dump_runs();

    uart.decode_runs();
    ret &= checkread(0x3E);
    ret &= checkread(0x3E);
    ret &= checkread(0xEF);
//    for (int i=0; i<5; i++)
//    {
//      printread();
//    }
  }
  if (ret) 
  {
    printf("success\n");
  }
  else
  {
    printf("FAIL\n");
  }
}

int main()
{
  test_partial_writes();
  test_entire_writes();
  test_rle();
}
