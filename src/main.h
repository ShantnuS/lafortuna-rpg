#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "switches.h"

#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7) 
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

#define COMPONENTS	2;
#define NAME_LENGTH 12;
#define MAX_ENTITIES 10;

typedef char Name[12];
typedef int(*IntFunc)(void);

typedef struct
{
	int16_t noOptions;
	int16_t optionNo;
	char ** optionNames;
	char * windowName;
	IntFunc * functions;
} window;

typedef struct
{
	float pos[2];
	float vel[2];
	int16_t colour;
} entity;

void showWin1();
void toggleLED();