#include "Buttonscroll.h"
#include "arduino.h"

//Constructor & Destructor
//======================================================
		Button::Button()
		{
			scrollIndex = 0;
			buttonCounter = 0;
			button_is_on = 0;
		}

		Button::~Button()
		{

		}

// Srcoll trough index
//======================================================
unsigned char	Button::ButtonScroll(unsigned char maximum, unsigned char buttonread, unsigned char minButtonTime)
		{
			buttonState = buttonread;									 //read if button is pushed or not
				
			if(buttonState == 1 && button_is_on == 0){ //button is not pushed
					button_is_on = 1;
					buttonCounter = 0;
					buttonCounter = millis();
			}
			if(buttonState == 0 && (millis() - buttonCounter) >= minButtonTime && button_is_on == 1){ 
					scrollIndex  = ((scrollIndex + 1) % maximum); 
					button_is_on = 0;
					buttonCounter = 0; 
				}
			if(buttonState == 0){
					button_is_on = 0;
					buttonCounter = 0; 
			}
			
			return scrollIndex;
		}

