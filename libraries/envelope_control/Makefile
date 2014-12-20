clean:
	rm -rf *.o *.dSYM EnvelopeTestbench

native: Envelope.cpp ../EnvelopeTestbench.cpp
	rm -rf *.o *.dSYM
	g++ Envelope.cpp -DNODUINO -Wall -c -I. -ggdb
	g++ ../EnvelopeTestbench.cpp -Wall -ggdb -I. -c 
	g++ Envelope.o EnvelopeTestbench.o -o EnvelopeTestbench

avr: Envelope.cpp ../EnvelopeTestbench.cpp
	rm -rf *.o *.dSYM
	avr-g++ Envelope.cpp -Wall -c -I. -ggdb
	avr-g++ ../EnvelopeTestbench.cpp -Wall -ggdb -I. -c 
	avr-g++ Envelope.o EnvelopeTestbench.o -o EnvelopeTestbench

