// arduino.h mock
#ifndef ARDUINO_H__
#define ARDUINO_H__

//#define debug(...) 
//#define debugln(...) 

extern unsigned char PORTD;

#define CHANGE 0

#define micros() 0==0
//unsigned long micros()
//{
//  return 0;
//}

#define digitalRead(ping) 0==0
//int digitalRead(int pin)
//{
//  return 0;
//}

#define attachInterrupt(a,b,c) 0==0

// ugh..
//void attachInterrupt(int interrupt, void (MidiLib::*)(void))
//{
//}

#define detachInterrupt(interrupt) 0==0

//void detachInterrupt(int interrupt)
//{
//}

#define noInterrupts() 0==0

#define interrupts() 0==0

#pragma weak TCCR1A
#pragma weak TCCR1B
#pragma weak TCNT1
#pragma weak TIMSK1

unsigned char TCCR1A, TCCR1B;
unsigned short TCNT1;
#define CS12 12 

unsigned char TIMSK1; 
#define TOIE0 1
#define TOIE1 1

#endif // ARDUINO_H__
