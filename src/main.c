#include "main.h"


const int8_t tile_size = 20, room_size = 20;

int8_t 
	player_x = 1, player_y = 1, old_player_x = 1, old_player_y = 1,
	room_x = 0, room_y = 1;

const size_t s = sizeof(char) * 16;
char * buffer;

uint8_t life = 10, att = 1, str = 1, stm = 1, def = 1, wis = 1, inte = 1;
uint16_t xp = 0, money = 42, level = 1;
char name[6] = "malloc";

void quick_debug_print(int c)
{
	sprintf(buffer, "%d", c);
	display_string(buffer);
}

inline uint8_t get_player_tile(uint8_t x, uint8_t y)
{
	return room_data[room_x][room_y][x + y * 16];
}

void draw_room(uint8_t x, uint8_t y)
{
	int i, j,c =0;
	// room tiles
	for (i = 0; i < 16; i++)
		for (j = 0; j < 11; j++, c++)
			fill_sprite6(i * tile_size, j * tile_size, tile_size, tile_data[get_player_tile(i,j)]);
}

int redraw()
{
	// player tiles
	overlay_sprite6(player_x * tile_size, player_y * tile_size, tile_size, tile_data[player]);

	// redraw previous walked tile
	fill_sprite6(old_player_x * tile_size, old_player_y * tile_size, tile_size, tile_data[get_player_tile(old_player_x, old_player_y)]);

	// on screen text
	sprintf(buffer, "Player Coord:     %d, %d", player_x, player_y);
	display_string_xy(buffer, 0,0);
	sprintf(buffer, "Old Player Coord: %d, %d", old_player_x, old_player_y);
	display_string_xy(buffer, 0,8);
	sprintf(buffer, "Room Coord: %d, %d", room_x, room_y);
	display_string_xy(buffer, 0, 16);

	display.foreground = YELLOW;
	sprintf(buffer, "%-6s", name);
	display_string_xy(buffer, 3, tile_size * 11 + 2);
	display.foreground = WHITE;
	sprintf(buffer, "|Life:%-3i|XP:%-4i|%-4ic|Lv%-2i", life, xp, money,level);
	display_string_xy(buffer, 50, tile_size * 11 + 2);
	sprintf(buffer, "Stats|Att:%-2i|Str:%-2i|Stm:%-2i|Def:%-2i|Wis:%-2i|Int:%-2i", att, str, stm, def, wis, inte);
	display_string_xy(buffer, 3, tile_size * 11 + 10);

	return 1;
}

void draw_gui()
{
	int i, j;
	// gui tiles
	i = 0;
	j = tile_size * 11;
	for (; i < LCDHEIGHT; i += tile_size)
		fill_sprite6(i, j, tile_size, tile_data[gui_middle]);
	i = 0;
	fill_sprite6(i, j, tile_size, tile_data[gui_left]);
	i = tile_size * 15;
	fill_sprite6(i, j, tile_size, tile_data[gui_right]);
}
inline void store_player_pos()
{
	old_player_x = player_x;
	old_player_y = player_y;
}

int loop()
{
	uint8_t event_happened = 0, room_change = 0, current_tile = 0, walled = 0;
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
			if (player_x > 15)
			{
				player_x = 0;
				room_x++;
				room_change = 1;
			}
			if (player_x < 0)
			{
				player_x = 15;
				room_x--;
				room_change = 1;
			}
			if (player_y > 10)
			{
				player_y = 0;
				room_y++;
				room_change = 1;
			}
			if (player_y < 0)
			{
				player_y = 10;
				room_y--;
				room_change = 1;
			}
			if (room_change)
			{
				draw_room(room_x, room_y);
				draw_gui();
				redraw();
				room_change = 0;
			}	

			current_tile = get_player_tile(player_x, player_y);
			switch (current_tile)
			{
			case water:
				walled = 1;
				player_x = old_player_x;
				player_y = old_player_y;
				break;
			default:
				redraw();
			}

			
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

	buffer = malloc(s);	
	
	draw_room(room_x,room_y);
	draw_gui();
	redraw();
	loop();

	return 1;
}

