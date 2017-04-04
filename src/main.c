#include "main.h"

inline uint16_t w_getw(uint8_t x, uint8_t y, uint16_t t, uint8_t part)
{
	return _rw(&(w[x + y*world_size][t][part]));
}
inline uint8_t w_getb(uint8_t x, uint8_t y, uint16_t t, uint8_t part)
{
	return _rb(&(w[x + y*world_size][t][part]));
}

void print_debug_text()
{
	// on screen debug text
	sprintf(buffer, "Player Coord:     %d, %d", PLAYER.x, PLAYER.y);
	display_string_xy(buffer, 0, 0);
	sprintf(buffer, "Old Player Coord: %d, %d", PLAYER.x, PLAYER.y);
	display_string_xy(buffer, 0, 8);
	sprintf(buffer, "Room Coord: %d, %d", PLAYER.area_x, PLAYER.area_y);
	display_string_xy(buffer, 0, 16);
}
void print_gui_text()
{
	// gui
	display.foreground = YELLOW;
	sprintf(buffer, "%-6s", name);
	display_string_xy(buffer, 3, tile_size * 11 + 2);
	display.foreground = WHITE;
	sprintf(buffer, "|Life:%-3i|XP:%-4i|%-4ic|Lv%-2i", life, xp, money, level);
	display_string_xy(buffer, 50, tile_size * 11 + 2);
	sprintf(buffer, "Stats|Att:%-2i|Str:%-2i|Stm:%-2i|Def:%-2i|Wis:%-2i|Int:%-2i", att, str, stm, def, wis, inte);
	display_string_xy(buffer, 3, tile_size * 11 + 10);
}

void draw_gui()
{
	int i, j;
	// gui tiles
	i = 0;
	j = tile_size * 11;
	for (; i < LCDHEIGHT; i += tile_size)
		fill_sprite6(i, j, tile_size, other_data[gui_middle]);
	i = 0;
	fill_sprite6(i, j, tile_size, other_data[gui_left]);
	i = tile_size * 15;
	fill_sprite6(i, j, tile_size, other_data[gui_right]);
}
void draw_area(uint8_t x, uint8_t y)
{
	uint8_t t = 0, t_x, t_y, l, player_prq = 0, o_id, p_id, i;
	mob * m;
	object * o, * p;
	for (t_y = 0; t_y < area_size_y; ++t_y)
		for (t_x = 0; t_x < area_size_x; ++t_x, ++t)
		{
			player_prq = PLAYER.x == t_x && PLAYER.y == t_y;
			o_id = w_getw(x, y, t, _o_object);
			p_id = w_getw(x, y, t, _p_object);
			o = o_id == 0 ? 0 : &obj_arr[o_id - 1];
			p = p_id == 0 ? 0 : &obj_arr[p_id - 1];
			for (l = 0; l < 7; ++l)
			{
				if (l == player_layer)
				{
					for (i = 0; i < mob_amt; ++i)
					{
						m = &mob_arr[i];
						if (m->area_x == x && m->area_y == y && m->x == t_x && m->y == t_y)
							overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, mob_data[m->data_id][m->dir]);
					}
					if(player_prq)
						overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, mob_data[PLAYER.data_id][PLAYER.dir]);
				}
				else if (w_getb(x, y, t, _layer) == l)
				{
					overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, tile_data[w_getb(x, y, t, _data_id)]);					
				}
				else if (o != 0 && o->layer == l)
				{
					overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, object_display_data[o->data_id]);
				}
				else if (p != 0 && p->layer == l)
				{
					overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, object_display_data[p->data_id]);
				}
			}
		}
}
void redraw_tile(uint8_t a_x, uint8_t a_y, uint8_t x, uint8_t y)
{
	object * o, *p;
	mob * m;
	uint8_t t = x + y * area_size_x, t_x = x, t_y = y, l, player_prq = 0, o_id, p_id, i;
	player_prq = PLAYER.x == t_x && PLAYER.y == t_y;
	o_id = w_getw(a_x, a_y, t, _o_object);
	p_id = w_getw(a_x, a_y, t, _p_object);
	o = o_id == 0 ? 0 : &obj_arr[o_id - 1];
	p = p_id == 0 ? 0 : &obj_arr[p_id - 1];
	for (l = 0; l < 7; ++l)
	{
		if (l == player_layer)
		{
			for (i = 0; i < mob_amt; ++i)
			{
				m = &mob_arr[i];
				if (m->area_x == a_x && m->area_y == a_y && m->x == t_x && m->y == t_y)
					overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, mob_data[m->data_id][m->dir]);
			}
			if (player_prq)
				overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, mob_data[PLAYER.data_id][PLAYER.dir]);
		}
		else if (w_getb(a_x, a_y, t, _layer) == l)
		{
			overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, tile_data[w_getb(a_x, a_y, t, _data_id)]);
		}
		else if (o != 0 && o->layer == l)
		{
			overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, object_display_data[o->data_id]);
		}
		else if (p != 0 && p->layer == l)
		{
			overlay_sprite6(t_x * tile_size, t_y * tile_size, tile_size, object_display_data[p->data_id]);
		}
	}
}
void redraw_mob(mob * m)
{
	if(m->old_x != m->x || m->old_y != m->y)
		redraw_tile(m->area_x, m->area_y, m->old_x, m->old_y);
	redraw_tile(m->area_x, m->area_y, m->x, m->y);
}

int loop()
{
	uint8_t event_happened = 0, area_change = 0, speed = 0, i;
	int8_t d;
	mob * m;
	while (1) 
	{
		_delay_ms(50);
		if (down_pressed())
		{
			store_mob_pos(&PLAYER);
			PLAYER.x--;
			PLAYER.dir = DIR_L;
			event_happened = 1;
		}
		if (up_pressed())
		{
			store_mob_pos(&PLAYER);
			PLAYER.x++;
			PLAYER.dir = DIR_R;
			event_happened = 1;
		}
		if (left_pressed())
		{
			store_mob_pos(&PLAYER);
			PLAYER.y--;
			PLAYER.dir = DIR_B;
			event_happened = 1;
		}
		if (right_pressed())
		{
			store_mob_pos(&PLAYER);
			PLAYER.y++;
			PLAYER.dir = DIR_F;
			event_happened = 1;
		}
		if (center_pressed())
		{
			draw_area(area_x, area_y);
			draw_gui();
		}
		if (event_happened)
		{
			if(!(speed %mob_speed))
				for (i = 0; i < mob_amt; ++i)
				{
					m = &mob_arr[i];
					if (m->area_x == PLAYER.area_x && m->area_y == PLAYER.area_y)
					{
						store_mob_pos(m);
						d = PLAYER.old_x - m->x;
						if (d > 0)
						{
							m->x++;
							m->dir = DIR_R;
						}
						if (d < 0)
						{
							m->x--;
							m->dir = DIR_L;
						}

						d = PLAYER.old_y - m->y;
						if (d > 0)
						{
							m->y++;
							m->dir = DIR_F;
						}
						if (d < 0)
						{
							m->y--;
							m->dir = DIR_B;
						}

						if (w_getb(m->area_x, m->area_y, m->x + m->y*area_size_x, _blocked))
						{
							m->x = m->old_x;
							m->y = m->old_y;
						}
						else
						{
							redraw_mob(m);
						}
					}
				}
			speed = (speed + 1) % mob_speed;

			if (PLAYER.x > 15)
			{
				PLAYER.x = 0;
				PLAYER.area_x++;
				area_change = 1;
			}
			if (PLAYER.x < 0)
			{
				PLAYER.x = 15;
				PLAYER.area_x--;
				area_change = 1;
			}
			if (PLAYER.y > 10)
			{
				PLAYER.y = 0;
				PLAYER.area_y++;
				area_change = 1;
			}
			if (PLAYER.y < 0)
			{
				PLAYER.y = 10;
				PLAYER.area_y--;
				area_change = 1;
			}
			if (area_change)
			{
				draw_area(PLAYER.area_x, PLAYER.area_y);
				draw_gui();			
				area_change = 0;
			}

			if (w_getb(PLAYER.area_x, PLAYER.area_y, PLAYER.x + PLAYER.y*area_size_x, _blocked))
			{
				PLAYER.x = PLAYER.old_x;
				PLAYER.y = PLAYER.old_y;
			}
			else
			{
				redraw_mob(&PLAYER);
			}

			//// other ////
			print_gui_text();
			print_debug_text();
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
	lcd_brightness(25);
	
	DDRB |= _BV(PB7);
	PORTB &= ~_BV(PB7);
	
	/* enable button press inturrupt */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS10);
	TIMSK1 |= _BV(OCIE1A);

	OCR3A = 0;

	buffer = malloc(s);	
	draw_area(PLAYER.area_x, PLAYER.area_y);
	draw_gui();
	print_debug_text();
	loop();
	

	/*sprintf(buffer, "Player Coord:     %d, %d", -1, -1);
	display_string_xy(buffer, 0, 0);
	sprintf(buffer, "Old Player Coord: %d, %d", -1, -1);
	display_string_xy(buffer, 0, 8);
	sprintf(buffer, "Room Coord: %d, %d", -1, -1);
	display_string_xy(buffer, 0, 16);*/
	return 1;
}

