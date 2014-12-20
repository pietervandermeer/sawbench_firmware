#include <SuperPiggyPotObserver.h>

void SuperPiggyPotObserver::setEnvelope(SignalControl::Envelope& env)
{
	this->env = &env;
}

void SuperPiggyPotObserver::notify(int potIndex, uint16_t value)
{
	if (!env)
	{
		return;
	}

	switch(potIndex)
	{
	    case 0: 
	    	env->setAttack(value/8);
	    	break;
	    case 1: 
	    	env->setDecay(value/8);
	    	break;
	    case 2: 
	    	env->setSustainLevel(value/4);
	    	break;
	    case 3: 
	    	env->setRelease(value/8);
	    	break;
	}
}

