
#include "Writeregisters.h"
#include "arduino.h"

//Constructor & Destructor
//======================================================
	
		Writeregister::Writeregister()
		{
				//initialize registers:
				//====================================
				for(int x = 0; x < NUM_OF_REGISTERS; x++)
				{
					registers[x] = 0;
				}

		 ds_PIN = 8;     //Serial port input 74HC595
		 shcp_PIN = A4;   //Shift clock input 74HC595
		 stcp_PIN = A5;   //storage clock input 74HC595 

		}

		Writeregister::~Writeregister()
		{

		}

// write register for LED
//======================================================

void	Writeregister::LedWriter(unsigned char numOfPotstates, unsigned char LFO_index, unsigned char POT_index, unsigned char lfo=0)
			{
				LFO_index += numOfPotstates;
				
				//turn leds on/off	
				for(int x = 0; x < NUM_OF_REGISTERS; x++){
					 if(x == LFO_index || x == POT_index + 1){
						registers[x] = 1;      
					 }
					 else{
						registers[x] = 0;       
					 }
				 }
				 registers[0] = lfo;

				//write to register
					digitalWrite(stcp_PIN, LOW);
						
						for (int i = 0; i <  NUM_OF_REGISTERS; i++)
						{
						 digitalWrite(shcp_PIN, LOW);
						 digitalWrite(ds_PIN, registers[i]);  
						 digitalWrite(shcp_PIN, HIGH);
						}
							
					digitalWrite(stcp_PIN, HIGH);		
			}

