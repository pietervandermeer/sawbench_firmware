#include <iostream>
#include "SimpleEnvelope.h"

int main()
{
	SimpleEnvelope env;
	env.setAttack(20);
	env.setDecay(10);
	env.setSustainLevel(200);
	env.setRelease(100);
	env.trigger();
	std::cout << "attack step " << env.attackStep << "\n";
	std::cout << "decay step " << env.decayStep << "\n";
	std::cout << "sustain level " << env.sustainLevel << "\n";
	std::cout << "release step " << env.releaseStep << "\n";
	for (int i=0; i<500; i++)
	{
		if (i==100)
			env.stop();
		env.run();
		std::cout << env.output << "\n";
	}
	return 0;
}
