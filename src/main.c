#include "main.h"

float pos[2] = { 100, 100 };
float vel[2] = { 2, 2 };
int16_t colour = 0;
rectangle background;
entity * entities;
int16_t noEntites = 1;
int16_t maxEntities = 100;
int16_t rnd = 0;
window * win1;

int rand()
{
	return (rnd = (rnd * 109 + 89) % INT16_MAX);
}

void change(entity * e)
{
	//e->vel[0] *= 1.1f;
	//e->vel[1] *= 1.1f;
	e->colour = rand();
}



void showWindow(window * win)
{
	fill_rectangle(background, BLACK);
	char buff[40] = {};
	snprintf(buff, sizeof(buff),
		">>> %s", win->windowName);
	display_string_xy(buff, 0, 0);

	while (1)
	{		
		fill_rectangle(background, BLACK);
		_delay_ms(15);	
		int i = 0;
		for (i; i < win->noOptions; ++i)
		{
			if (win->optionNo == i)
				snprintf(buff, sizeof(buff),
					" > %-37s", win->optionNames[i]);
			else
				snprintf(buff, sizeof(buff),
					"%-40s", win->optionNames[i]);
			display_string_xy(buff, 0, 3 + (i + 1) * 9);
		}
		if (center_pressed())
		{
			if (win->optionNo == 0)
				return;
			int j = 1;
			for (j; j < win->noOptions; ++j)
				if(!(win->functions[j] == 0))
					win->functions[j]();
		}
		else if (down_pressed())
			win->optionNo += win->optionNo == win->noOptions - 1 ? 0 : 1;
		else if (up_pressed())
			win->optionNo += win->optionNo == 0 ? 0 : -1;
			
	}
}

void toggleLED()
{
	LED_TOGGLE;
}
void showWin1()
{
	showWindow(win1);
}

void doNothing() {};

int loop()
{
	size_t s = sizeof(char) * 16;
	char * buffer = malloc(s);
	while (1) 
	{
		_delay_ms(35);
		int i;
		for (i = 0; i < noEntites; ++i)
		{
			rectangle old_obj = { 
				(int)entities[i].pos[0],(int)entities[i].pos[0] + 5 ,
				(int)entities[i].pos[1],(int)entities[i].pos[1] + 5 };
			fill_rectangle(old_obj, BLACK);

			entities[i].pos[0] += entities[i].vel[0];
			entities[i].pos[1] += entities[i].vel[1];
			if (entities[i].pos[0] >= display.width || entities[i].pos[0] <= 0)
			{
				entities[i].vel[0] *= -1;
				change(&entities[i]);
			}
			if (entities[i].pos[1] >= display.height || entities[i].pos[1] <= 0)
			{
				entities[i].vel[1] *= -1;
				change(&entities[i]);
			}
			rectangle new_obj = {
				(int)entities[i].pos[0],(int)entities[i].pos[0] + 5 ,
				(int)entities[i].pos[1],(int)entities[i].pos[1] + 5 };
			fill_rectangle(new_obj, entities[i].colour);
		}
		if (center_pressed())
		{
			//noEntites++;
			//testWindow();
		}	
		sprintf(buffer, "%d", noEntites);
		display_string_xy(buffer, 0, 0);
	}
	
	return 1;
}

void initEntites()
{
	size_t size = sizeof(entity)*maxEntities;
	entities = malloc(size);
	int i;
	for (i = 0; i < maxEntities; ++i)
	{
		entities[i].colour = rand();
		entities[i].pos[0] = rand() % display.width;
		entities[i].pos[1] = rand() % display.height;
		entities[i].vel[0] = rand() % 6 - 3;
		entities[i].vel[1] = rand() % 6 - 3;
	}
}

int main()
{
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
	init_lcd();
	init_switches();
	set_frame_rate_hz(61);
	set_orientation(North);
	
	DDRB |= _BV(PB7);
	PORTB &= ~_BV(PB7);
	
	/* enable button press inturrupt */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);

	OCR3A = 0;

	const int16_t options1 = 3;
	const char windowName1[12] = "Menu2";
	const char * names1[12];
	names1[0] = "Back";
	names1[1] = "Nothing";
	names1[2] = "LED";
	IntFunc functions1[options1];
	functions1[0] = doNothing;
	functions1[1] = doNothing;
	functions1[2] = toggleLED;
	window temp = { options1, 0, names1, &windowName1, &functions1 };
	win1 = &temp;

	const int16_t options = 3;
	const char windowName[12] = "Menu1";
	const char * names[12];
	names[0] = "Back";
	names[1] = "Menu2";
	names[2] = "LED";
	IntFunc functions[options];
	functions[0] = doNothing;
	functions[1] = showWin1;
	functions[2] = toggleLED;
	window win = { options, 0, names, &windowName, &functions };

	showWindow(&win);


	display_string_xy("\n",0,0);
	display_string("size: ");

	do{
		while (!center_pressed()) { rnd += (rnd >= (INT16_MAX)) ? ((rnd = 0) + 1) : 1; }
		initEntites();
		rectangle temp = { 0,display.width,0,display.height };
		background = temp;
		display.background = BLACK;
		display.foreground = BLACK;
		//fill_rectangle(background, display.background);
		//loop();
		//testWindow();
	} while (1);
}