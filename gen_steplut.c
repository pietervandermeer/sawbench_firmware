#include <stdio.h>
#include <math.h>

int main()
{
	for (int i=0; i<128; i++)
	{
		double in = (40./128) + i/220.;

		//double out = sqrt(1.-in);
		double out = pow(1. - in, 3.) * 2.; 

		int val_int = (out*65535) + .5;
		printf("0x%04X,\n", val_int);
	}
	return 0;
}