/*==================================================================
Buttonscroll Class by Koen Pepping

This class was designed to read out buttons from arduino and
scroll trough different indexes.

On the sawbench this is used to scroll trough:

4 different LFO waves

3 different potstate destinations

arguments:

there is a maximum scroll range, for example the 4 LFO waves.

The buttonread should be a digitalRead(pin with pulldown button);

The button reacts when you release it 
after you pressed it for the minimal waiting time.

*///================================================================


class Button
{
		public:

			Button();
			~Button();

	unsigned char ButtonScroll(unsigned char maximum, unsigned char buttonread, unsigned char minButtonTime);

			unsigned char scrollIndex;

		private:

			unsigned char buttonState;
			unsigned long buttonCounter;
			unsigned char button_is_on;

};
