#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>

#define SWN     PC2
#define SWE     PC3
#define SWS     PC4
#define SWW     PC5
#define SWC     PE7

#define COMPASS_SWITCHES (_BV(SWW)|_BV(SWS)|_BV(SWE)|_BV(SWN))

extern int down;

typedef enum { UP, LEFT, RIGHT, DOWN, CENTRE, L } buttons;

void init_switches();
int center_pressed();
int left_pressed();
int right_pressed();
int up_pressed();
int down_pressed();
int down_held();

void run_event_capture();
int button_down_event(int button_code);
int button_down(int button_code);