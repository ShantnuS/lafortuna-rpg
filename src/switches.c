#include "switches.h"

int center_down = 0;
int left_down = 0;
int right_down = 0;
int up_down = 0;
int down_down = 0;
int down = 0;

int * oldDown;
int * newDown;

void init_switches(){
	DDRE &= ~_BV(SWC);
	PORTE |= _BV(SWC);
	DDRC &= ~_BV(SWN) & ~_BV(SWE) & ~_BV(SWS) & ~_BV(SWW);
	PORTC |= _BV(SWN) | _BV(SWE) | _BV(SWS) | _BV(SWW);

	DDRE &= ~_BV(SWC);   /* Central button */
	PORTE |= _BV(SWC);
	
	DDRC &= ~COMPASS_SWITCHES;  /* configure compass buttons for input */
	PORTC |= COMPASS_SWITCHES; 

	EICRB |= _BV(ISC40) | _BV(ISC50) | _BV(ISC71);

	size_t size = sizeof(int) * L;
	oldDown = malloc(size);
	newDown = malloc(size);
}

int is_down()
{
	int i;
	for (i = 0; i < L; ++i)
		if (newDown[i])
			return 1;
	return 0;
}
void run_event_capture()
{	
	int i;	for (i = 0; i < L; ++i)
		oldDown[i] = newDown[i];	

	newDown[CENTRE]		= ~PINE & _BV(PE7);
	newDown[UP]			= ~PINE & _BV(SWE);
	newDown[LEFT]		= ~PINE & _BV(SWN);
	newDown[DOWN]		= ~PINE & _BV(SWW);
	newDown[RIGHT]		= ~PINE & _BV(SWS);

	down = is_down();
}
int button_down_event(int button_code)
{
	return newDown[button_code] && !oldDown[button_code];
}
int button_down(int button_code)
{
	return newDown[button_code];
}

int center_pressed(){
	if ((~PINE & _BV(PE7)) && !center_down && !down) {
		center_down = 1;
		down = 1;
		return 1;
	} else if ((PINE & _BV(PE7)) && center_down){
		center_down = 0;
		down = 0;
	}
	return 0;
}
int left_pressed(){
	if ((~PINC & _BV(SWN)) && !left_down && !down) {
		left_down = 1;
		down = 1;
		return 1;
	} else if ((PINC & _BV(SWN))&& left_down){
		left_down = 0;
		down = 0;
	}
	return 0;
}
int right_pressed(){
	if ((~PINC & _BV(SWS)) && !right_down && !down) {
		right_down = 1;
		down = 1;
		return 1;
	} else if ((PINC & _BV(SWS)) && right_down){
		right_down = 0;
		down = 0;
	}
	return 0;
}
int up_pressed(){
	if ((~PINC & _BV(SWE)) && !up_down && !down) {
		up_down = 1;
		down = 1;
		return 1;
	} else if ((PINC & _BV(SWE)) && up_down){
		up_down = 0;
		down = 0;
	}
	return 0;
}
int down_pressed(){
	if ((~PINC & _BV(SWW)) && !down_down && !down) {
		down_down = 1;
		down = 1;
		return 1;
	} else if ((PINC & _BV(SWW)) && down_down){
		down_down = 0;
		down = 0;
	}
	return 0;
}
int down_held(){
	if ((~PINC & _BV(SWW)) && !down) {
		return 1;
		down = 1;
	} else {
		down = 0;
	}
	return 0;
}