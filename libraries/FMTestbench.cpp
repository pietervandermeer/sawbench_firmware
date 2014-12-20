#include <iostream>
#include <cmath>
#include "FrequencyModulator.h"

uint32_t fakeMicros;

uint32_t getFakeMicros()
{
  return fakeMicros;
}

uint16_t modFreq = 100;
uint16_t loopFreq = 2000;

SignalControl::FrequencyModulator fm(&getFakeMicros, modFreq, loopFreq);

uint16_t fm_depth;
uint8_t mod_index;
int16_t amp;

bool testFineTune()
{
  fm.setFreq(100, 2000);
  for (int i=0; i<100; i++)
  {
    //amp = fm.getAmp();
    //amp = (int16_t) fm.step();
    //amp = static_cast<int16_t>(fm.step());
    amp = fm.step();
    //std::cout << " amp=" << amp << "\n";
  }
  std::cout << " amp=" << amp << "\n";
  uint16_t amp100 = amp;
  std::cout << "----\n";

  fm.setFreq(101, 2000);
  for (int i=0; i<100; i++)
  {
    //amp = fm.getAmp();
    //amp = (int16_t) fm.step();
    //amp = static_cast<int16_t>(fm.step());
    amp = fm.step();
    //std::cout << " amp=" << amp << "\n";
  }
  std::cout << " amp=" << amp << "\n";

  return amp100 != amp;
}

bool testHighFreq()
{
  fm.setFreq(1500, 3300);
  for (int i=0; i<100; i++)
  {
    amp = fm.step();
    std::cout << amp << "\n";
  }
 
  return true;
}

int main()
{
  bool ret = true;
  std::cout << "\n";
  ret &= testFineTune();
  std::cout << "\n";
  ret &= testHighFreq();

  if (ret)
  {
    std::cout << "success\n";
    return 0;
  }
  else
  {
    std::cout << "FAIL!\n";
    return -1;
  }
}
