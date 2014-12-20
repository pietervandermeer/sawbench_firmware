#include <Controls.h>

// allows arbitrary order and balancing (i.e. more or less of one specific type per period)
const TControlState controlOrderArray[] = 
{
  CONTROL_STATE_VCO_BUTTON, 
  CONTROL_STATE_ADSR_BUTTON, 
  CONTROL_STATE_POT0, 
  CONTROL_STATE_POT1, 
  CONTROL_STATE_POT2, 
  CONTROL_STATE_POT3
};

void Controls::setVco(Vco& vco)
{
  this->vco = &vco;
}

void Controls::setPotObserver(PotObserver& pot)
{
  this->potObserver = &pot;
}

void Controls::readout()
{
  TControlState controlState = controlOrderArray[controlStateIndex++];
  
  if (controlState == CONTROL_STATE_VCO_BUTTON)
  {
    int toggleVcoMode = digitalRead(vcoPin);
    if (toggleVcoMode < oldToggleVco)
    { // button pressed = 0, hence the lower-than operator
      vco->switchVcoMode();
    }
    oldToggleVco = toggleVcoMode;
  } 
  else if (controlState == CONTROL_STATE_ADSR_BUTTON) 
  {
    int toggleAdsrMode = digitalRead(adsrPin);  
    adsrMode ^= (toggleAdsrMode < oldToggleAdsr); // button pressed = 0, hence the lower-than operator
    oldToggleAdsr = toggleAdsrMode;
  }
  // from left to right. 
  // pot R38: A3
  // pot R37: A2
  // pot R36: A5
  // pot R34: A4
  else if (controlState == CONTROL_STATE_POT0) 
  {
    controlPot0 = analogRead(3);
    if (potObserver) 
    {
      potObserver->notify(0, controlPot0);
    }
  }
  else if (controlState == CONTROL_STATE_POT1) 
  {
    controlPot1 = analogRead(2);
    if (potObserver) 
    {
      potObserver->notify(1, controlPot1);
    }
  }
  else if (controlState == CONTROL_STATE_POT2) 
  {
    controlPot2 = analogRead(5);
    if (potObserver) 
    {
      potObserver->notify(2, controlPot2);
    }
  }
  else if (controlState == CONTROL_STATE_POT3) 
  {
    controlPot3 = analogRead(4);
    if (potObserver) 
    {
      potObserver->notify(3, controlPot3);
    }
  }

  if (controlStateIndex > sizeof(controlOrderArray)/sizeof(controlOrderArray[0]))
  {
    controlStateIndex = 0;
  }
}
