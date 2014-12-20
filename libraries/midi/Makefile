clean:
	rm -rf *.o *.dSYM

unittest: MidiUart.cpp Statemachine.cpp ../MidiUartTestbench.cpp ../MidiStatemachineTestbench.cpp
	rm -rf *.o *.dSYM
	g++ MidiUart.cpp -DNODUINO -Wall -c -I. -ggdb
	g++ Statemachine.cpp -DNODUINO -Wall -c -I. -ggdb
	gcc Arduino.c -DNODUINO -Wall -c -I. -ggdb
	g++ ../MidiUartTestbench.cpp -Wall -ggdb -I. -c
	g++ ../MidiStatemachineTestbench.cpp -Wall -ggdb -I. -c
	g++ Arduino.o Statemachine.o MidiUart.o MidiStatemachineTestbench.o -o MidiStatemachineTestbench
	g++ Arduino.o MidiUart.o MidiUartTestbench.o -o MidiUartTestbench

#avr:
#	avr-g++
