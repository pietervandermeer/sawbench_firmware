#include "Buttonscroll.h"
#include "arduino.h"

//Constructor & Destructor
//======================================================
Button::Button(uint32_t* millisPtr)
{
	this->millisPtr = millisPtr;
	scrollIndex = 0;
	buttonCounter = 0;
	button_is_on = 0;
}

Button::~Button()
{

}

// Srcoll trough index
//======================================================
unsigned char Button::ButtonScroll(unsigned char maximum, unsigned char buttonread, unsigned char minButtonTime)
{
	buttonState = !buttonread;									 //read if button is pushed or not (pulled up, so inverse!)
	
	// wait for release after special feature has been activated
	if(waitForRelease)
	{
		if (buttonState == 0)
		{
			waitForRelease = false;
			button_is_on = 0;
			buttonCounter = 0;
		}
		return scrollIndex;
	}

	if(buttonState == 1 && button_is_on == 0){ //button is not pushed
			button_is_on = 1;
			buttonCounter = 0;
			buttonCounter = *millisPtr;
	}
	// pressed for 3 seconds? -> activite special feature!
	else if(buttonState == 1 && (*millisPtr - buttonCounter) >= 3000 && button_is_on == 1){ 
			waitForRelease = true; 
			return 255;
	}
	// pressed long enough and released
	else if(buttonState == 0 && (*millisPtr - buttonCounter) >= minButtonTime && button_is_on == 1){ 
			scrollIndex  = ((scrollIndex + 1) % maximum); 
			button_is_on = 0;
			buttonCounter = 0; 
	}
	else if(buttonState == 0){
			button_is_on = 0;
			buttonCounter = 0; 
	}
	
	return scrollIndex;
}
