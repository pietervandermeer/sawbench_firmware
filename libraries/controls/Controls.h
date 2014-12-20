#ifndef PIGGY_CONTROLS_H
#define PIGGY_CONTROLS_H

#include <stdint.h>
#include "Arduino.h"
#include "Vco.h"
#include "PotObserver.h"

enum TControlState
{
  CONTROL_STATE_VCO_BUTTON
 ,CONTROL_STATE_ADSR_BUTTON
 ,CONTROL_STATE_POT0 
 ,CONTROL_STATE_POT1
 ,CONTROL_STATE_POT2 
 ,CONTROL_STATE_POT3 
};

class Controls
{
public:
	Controls(const int vcoModeInputPin, const int adsrModeInputPin) 
	{
		vcoPin = vcoModeInputPin;
		adsrPin = adsrModeInputPin;
		controlStateIndex = 0;
		oldToggleAdsr = 0;
		oldToggleVco = 0;
		adsrMode = 0;
		pinMode(vcoPin, INPUT_PULLUP); 
  		pinMode(adsrPin, INPUT_PULLUP);
	}
	~Controls() {}
	void readout();
	void setVco(Vco& vco);
	void setPotObserver(PotObserver &pot);
	
	int vcoPin;
	int adsrPin;
	int adsrMode;
	int oldToggleAdsr;
	int oldToggleVco;

	uint16_t controlPot0;
	uint16_t controlPot1;
	uint16_t controlPot2;
	uint16_t controlPot3;

protected:
	PotObserver *potObserver;
	Vco* vco;
	unsigned int controlStateIndex;

};

#endif //PIGGY_CONTROLS_H
