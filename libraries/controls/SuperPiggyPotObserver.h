#ifndef SUPERPIGGYPOTOBSERVER_H
#define SUPERPIGGYPOTOBSERVER_H

#include <stdint.h>
#include <PotObserver.h>
#include <Envelope.h>

class SuperPiggyPotObserver : public PotObserver
{
public: 
	SuperPiggyPotObserver() {}
	~SuperPiggyPotObserver() {}
	void setEnvelope(SignalControl::Envelope& env);
	void notify(int potIndex, uint16_t value);

protected:
	SignalControl::Envelope* env;
};

#endif // SUPERPIGGYPOTOBSERVER_H
