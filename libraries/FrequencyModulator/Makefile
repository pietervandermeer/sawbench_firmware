native: FrequencyModulator.cpp FrequencyModulator.h ../FMTestbench.cpp
	rm -rf *.o *.dSYM
	g++ FrequencyModulator.cpp -DNODUINO -Wall -c -I. -ggdb
	g++ ../FMTestbench.cpp -Wall -ggdb -I. -c
	g++ FrequencyModulator.o FMTestbench.o -o FMTestbench

avr: FrequencyModulator.cpp FrequencyModulator.h ../FMTestBench.cpp
	rm -rf *.o *.dSYM
	avr-g++ FrequencyModulator.cpp -Wall -c -I. -ggdb
	avr-g++ ../FMTestBench.cpp -Wall -ggdb -I. -c
	avr-g++ FrequencyModulator.o FMTestBench.o -o FMTestbench

clean:
	rm -rf *.o *.dSYM FMTestbench a.out

