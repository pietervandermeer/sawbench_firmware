#ifndef PIGGY_VCO_H
#define PIGGY_VCO_H

#include <stdint.h>
#include "Arduino.h"

// TODO: use pvm library?

class Vco
{
public:
	Vco(int msPwmPin, int lsPwmPin) 
	{
		this->msPwmPin = msPwmPin;
		this->lsPwmPin = lsPwmPin;
		pinMode(msPwmPin, OUTPUT);
		pinMode(lsPwmPin, OUTPUT);
		vcoMode = VCO_MODE_SAW;
		//vcoMode = VCO_MODE_SQUARE;
	}
	~Vco() {}

	void calibrate();
	void write(uint16_t);
	uint16_t getNotePitch(uint8_t midi_note);

	enum VcoModeType
	{
	     VCO_MODE_SAW
		,VCO_MODE_SQUARE
	};
	VcoModeType vcoMode;

	enum VcoType
	{
		 VCO_TYPE_PIGGY      // saw only
		,VCO_TYPE_SUPERPIGGY // saw and rectangle (manual pwm)
		,VCO_TYPE_SAWBENCH   // saw only, but different offset
	};

	void setType(VcoType vcoType);
	void switchVcoMode();
 
protected:
	uint16_t inputOffset;
	int msPwmPin;           // most significant VCO pwm pin
	int lsPwmPin;           // least significant VCO pwm pin
};

#endif //PIGGY_VCO_H
