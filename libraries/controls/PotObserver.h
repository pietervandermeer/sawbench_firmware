#ifndef POTOBSERVER_H
#define POTOBSERVER_H

#include <stdint.h>

class PotObserver
{
public:
//	virtual PotObserver() {}
//	virtual ~PotObserver() {}
	virtual void notify(int potIndex, uint16_t value) = 0;
};

#endif // POTOBSERVER_H