#include "main.h"


const int8_t tile_size = 20, room_size = 20;

uint16_t colors[400];


int8_t player_x = 1, player_y = 1, old_player_x = 1, old_player_y = 1;
rectangle background, test = { 0, 19 ,0, 19 };
rectangle player_sprite;

const size_t s = sizeof(char) * 16;
char * buffer;

void draw_room()
{
	int i, j;
	// room tiles
	for (i = 0; i < LCDHEIGHT; i += tile_size)
		for (j = 0; j < LCDWIDTH; j += tile_size)
		{
			fill_sprite(i, j, tile_size, test_tile);
		}
}

void draw_gui()
{
	int i, j;
	// gui tiles
	i = 0;
	j = tile_size * 11;
	for (; i < LCDHEIGHT; i += tile_size)
		fill_sprite(i, j, tile_size, gui_middle);
	i = 0;
	fill_sprite(i, j, tile_size, gui_left);
	i = tile_size * 15;
	fill_sprite(i, j, tile_size, gui_right);
}

int redraw()
{
	// player tiles
	overlay_sprite(player_x * tile_size, player_y * tile_size, tile_size, player, test_tile);

	// redraw previous walked tile
	fill_sprite(old_player_x * tile_size, old_player_y * tile_size, tile_size, test_tile);

	// on screen text
	sprintf(buffer, "Player Coord:     %d, %d", player_x, player_y);
	display_string_xy(buffer, 3, tile_size * 11 + 2);
	sprintf(buffer, "Old Player Coord: %d, %d", old_player_x, old_player_y);
	display_string_xy(buffer, 3, tile_size * 11 + 10);

	return 1;
}

inline void store_player_pos()
{
	old_player_x = player_x;
	old_player_y = player_y;
}

int loop()
{
	int event_happened = 0;
	while (1) 
	{
		_delay_ms(50);
		if (down_pressed())
		{
			store_player_pos();
			player_x--;
			event_happened = 1;
		}
		if (up_pressed())
		{
			store_player_pos();
			player_x++;
			event_happened = 1;
		}
		if (left_pressed())
		{
			store_player_pos();
			player_y--;
			event_happened = 1;
		}
		if (right_pressed())
		{
			store_player_pos();
			player_y++;
			event_happened = 1;
		}
		if (event_happened)
		{
			redraw();
			event_happened = 0;
		}
	}
	
	return 1;
}

int main()
{
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
	init_lcd();
	init_switches();
	set_frame_rate_hz(61);
	set_orientation(West);
	
	DDRB |= _BV(PB7);
	PORTB &= ~_BV(PB7);
	
	/* enable button press inturrupt */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);

	OCR3A = 0;

	rectangle temp = { 0,display.width,0,display.height };
	background = temp;

	int i;
	for (i = 0; i < 400; i++)
		colors[i] = ORANGE_1;

	buffer = malloc(s);
	
	
	draw_room();
	draw_gui();
	redraw();
	loop();

	return 1;
}

