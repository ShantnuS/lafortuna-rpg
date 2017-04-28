#include "main.h"

// gets bytes from the world tile data
inline uint16_t w_getw(uint8_t x, uint8_t y, uint16_t t, uint8_t part)
{
	return _rw(&(w[x + y*WORLD_SIDE_SIZE][t][part]));
}
inline uint8_t w_getb(uint8_t x, uint8_t y, uint16_t t, uint8_t part)
{
	return _rb(&(w[x + y*WORLD_SIDE_SIZE][t][part]));
}

// draw the gui tiles and gui information
void draw_gui(uint8_t draw_frame)
{
	if (draw_frame)
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
	// gui
	display.foreground = GREEN;
	sprintf(buffer, "%-6s", user.name);
	display_string_xy(buffer, 3, tile_size * 11 + 2);
	display.foreground = WHITE;
	sprintf(buffer, "|Life %-3d|XP %-4d|%-4dc|Lvl %-2d", user.life, user.xp, user.money, user.lvl);
	display_string_xy(buffer, 50, tile_size * 11 + 2);
}

// draws a tile on the screen
void redraw_tile(uint8_t a_x, uint8_t a_y, uint8_t x, uint8_t y)
{
	uint8_t
		t = x + y * area_size_x,
		p_prq = user.x == x && user.y == y,
		a = (a_x << 4) | (a_y & 0xf),
		xy = (x << 4) | (y & 0xf);

	uint8_t
		p2 = w_getb(a_x, a_y, t, 1);
	//p3 = w_getb(a_x, a_y, t, 2);

	//     ot opt op o

	// 
	uint8_t
		layers[4] =
	{
		(p2 & 0xf),
		(p2 & 0xf0) >> 4,
		0,
		0
	},
		mask[4] =
	{
		0,
		(p2 & 0x3f),
		0,
		0
	};

	uint16_t l, o, m;
	for (l = 0; l < 4; ++l)
	{
		for (o = 0; o < WORLD_OBJECTS_AMT; ++o)
		{
			if ((_rb(&world_objects[o][2]) & 0xc0) >> 6 == l)
			{
				if (_rb(&world_objects[o][0]) == a && _rb(&world_objects[o][1]) == xy)
					overlay_sprite6(x * tile_size, y * tile_size, tile_size, world_object_data[_rb(&world_objects[o][2]) & 0x3f]);
			}
		}
		if (l == 1)
		{
			for (m = 0; m < NPC_AMT; ++m)
			{
				if ((_rb(&npcs[m][2]) & 0x40) == 0x40)
				{
					if ((combat_npcs[m][5] & 0x4) == 0x4)
						if (_rb(&npcs[m][0]) == a && combat_npcs[m][0] == xy)
							overlay_sprite6(x * tile_size, y * tile_size, tile_size, npc_data[_rb(&npcs[m][2]) & 0xf][combat_npcs[m][5] & 0x3]);
				}
				else
				{
					if (_rb(&npcs[m][0]) == a && _rb(&npcs[m][1]) == xy)
						overlay_sprite6(x * tile_size, y * tile_size, tile_size, npc_data[_rb(&npcs[m][2]) & 0xf][(_rb(&npcs[m][2]) & 0xc0) >> 4]);
				}
			}
			if (p_prq)
				overlay_sprite6(x * tile_size, y * tile_size, tile_size, npc_data[user.data_id][user.dir]);
		}
		if (layers[l] != 0)
		{
			if (l == 0)
			{
				fill_sprite6(x * tile_size, y * tile_size, tile_size, tile_data[layers[l]]);
			}
			else if (l == 1)
			{
				mask_sprite6(x * tile_size, y * tile_size, tile_size, tile_data[layers[l]], mask_data[(mask[l] & 0x7)], mask[l]);
			}
		}
	}


}

void draw_area(uint8_t x, uint8_t y)
{
	display.foreground = WHITE;
	display_string_xy("loading...", 260, 212);
	uint8_t t_x, t_y;
	for (t_y = 0; t_y < area_size_x; ++t_y)
		for (t_x = 0; t_x < area_size_y; ++t_x)
			redraw_tile(x, y, t_y, t_x);
}

void redraw_user()
{
	redraw_tile(user.area_x, user.area_y, user.old_x, user.old_y);
	redraw_tile(user.area_x, user.area_y, user.x, user.y);
}

void print_debug_text()
{
	// on screen debug text
	sprintf(buffer, "user Coord:     %d, %d", user.x, user.y);
	display_string_xy(buffer, 0, 0);
	sprintf(buffer, "Old user Coord: %d, %d", user.x, user.y);
	display_string_xy(buffer, 0, 8);
	sprintf(buffer, "Room Coord: %d, %d", user.area_x, user.area_y);
	display_string_xy(buffer, 0, 16);
}

void end_battle(uint8_t id)
{
	int8_t redraw = 1, dinged = 0, selected_item = 0;

	clear_screen();

	combat_npcs[id][5] ^= combat_npcs[id][5] & 0x4;
	uint8_t xp_loot = ((combat_npcs[id][5] & 0xf0) >> 4) * combat_npcs[id][3] * combat_npcs[id][4] / 50;
	user.xp += xp_loot;
	if (user.xp >= level_xp(user.lvl + 1))
	{
		user.lvl++;
		dinged = 1;
	}

	uint8_t coins_dropped = _rand() % (((combat_npcs[id][5] & 0xf0) >> 4) * 10);

	user.money += coins_dropped;

	uint8_t potential_loot[5] = {};
	int i;
	for (i = 0; i < 5; ++i)
	{
		if (!(_rand() % 4))
		{
			potential_loot[i] = _rand() % ITEMS_AMT;
		}
		else
		{
			i++;
			break;
		}
	}

	while (1)
	{
		_delay_ms(1);
		if (redraw)
		{
			display.foreground = YELLOW;
			sprintf(buffer, "xp loot: %d", xp_loot);
			display_string_xy(buffer,0,0*8);
			if (dinged)
			{
				sprintf(buffer, "you dinged!  now level: %d", user.lvl);
				display_string_xy(buffer,0,1*8);
			}
			sprintf(buffer, "coins looted: %d", coins_dropped);
			display_string_xy(buffer,0,2*8);

			sprintf(buffer, "loot: ");
			display_string_xy(buffer, 0, 4 * 8);

			uint8_t ii;
			for (ii = 0; ii < i; ++ii)
			{
				if (selected_item == ii)
				{
					display.foreground = GREEN;
				}
				else
				{
					display.foreground = YELLOW;
				}
				sprintf(buffer, "%-15s", potential_loot[ii] == 0xff ? "looted!" : item_names[potential_loot[ii]]);
				display_string_xy(buffer,0,(5+ii)*8);
			}

			if (selected_item == i)
			{
				display.foreground = GREEN;
			}
			else
			{
				display.foreground = YELLOW;
			}
			sprintf(buffer, "%-15s", "exit");
			display_string_xy(buffer,0,(5+ii+1)*8);
		}

		if (right_pressed())
		{
			selected_item++;
			selected_item %= i + 1;
			redraw = 1;
		}
		else if (left_pressed())
		{
			selected_item--;
			selected_item = selected_item < 0 ? i+1 : selected_item;
			redraw = 1;
		}
		else if (down_pressed())
		{
			redraw = 1;
		}
		else if (up_pressed())
		{
			redraw = 1;
		}
		else if (center_pressed())
		{
			if (selected_item == i)
				break;
			uint8_t items;
			for (items = 0; items < 16; ++items)
			{
				if (user.inventory[items][0] == potential_loot[selected_item])
				{
					user.inventory[items][1]++;
					potential_loot[selected_item] = 0xff;
				}
				else if (user.inventory[items][0] == 0xff && potential_loot[selected_item] != 0xff)
				{
					user.inventory[items][0] = potential_loot[selected_item];
					user.inventory[items][1]++;
					potential_loot[selected_item] = 0xff;
				}
			}

		}
	}
	draw_area(user.area_x, user.area_y);
	draw_gui(1);
}

void death()
{

}

void draw_inventory()
{
	clear_screen();

	int8_t redraw = 1, selected_item = 0;

	while (1)
	{
		_delay_ms(1);
		if (redraw)
		{
			if(user.inventory[selected_item][0] != -1)
				fill_sprite6_scaled(250, 10, tile_size, item_data[user.inventory[selected_item][0]], 3);

			sprintf(buffer, "%-15s", "INVENTORY");
			display_string_xy(buffer, 0, 0 * 8);

			uint8_t ii;
			for (ii = 0; ii < 16; ++ii)
			{
				if (selected_item == ii)
				{
					display.foreground = GREEN;
				}
				else
				{
					display.foreground = YELLOW;
				}
				sprintf(buffer, "%-15s   x%d", item_names[user.inventory[ii][0]], user.inventory[ii][1]);
				display_string_xy(buffer, 0, (2 + ii) * 8);
			}

			if (selected_item == 16)
			{
				display.foreground = GREEN;
			}
			else
			{
				display.foreground = YELLOW;
			}
			sprintf(buffer, "%-15s", "exit");
			display_string_xy(buffer, 0, (2 + ii + 1) * 8);
		}

		if (right_pressed())
		{
			selected_item++;
			selected_item %= 17;
			redraw = 1;
		}
		else if (left_pressed())
		{
			selected_item--;
			selected_item = selected_item < 0 ? 17 : selected_item;
			redraw = 1;
		}
		else if (down_pressed())
		{
			redraw = 1;
		}
		else if (up_pressed())
		{
			redraw = 1;
		}
		else if (center_pressed())
		{
			//if (selected_item == 16)
				break;
			
		}
	}
	draw_area(user.area_x, user.area_y);
	draw_gui(1);
}

void draw_combat_screen(uint8_t id)
{
	clear_screen();

	int8_t selected_att = 0,
		buff_timer = 0,
		buff_time = 0,
		last_att = -1,
		att = 0;
	uint8_t
		redraw = 1,
		user_turn = 1,
		lvl = (combat_npcs[id][5] & 0xf0) >> 4,
		life = combat_npcs[id][3],
		mana = combat_npcs[id][4],
		i;
	int16_t new_l = 0,
		new_l2 = 0;


	char last_turn[5][30] = {
		{""},
		{""},
		{""},
		{""},
		{"you have an encounter!"},
	};

	while (1)
	{
		_delay_ms(1);

		if (redraw)
		{
			fill_sprite6_scaled(250, 10, tile_size, npc_data[_rb(&npcs[id][2]) & 0xf][front], 3);

			display.foreground = GREEN;
			sprintf(buffer, "%-10s = lvl %-2i", user.name, user.lvl);
			display_string_xy(buffer, 0, 0);
			sprintf(buffer, "Life %-3i | Mana %-3i", user.life, user.mana);
			display_string_xy(buffer, 0, 1 * 8);
			sprintf(buffer, "STATS = str %-2i|stm %-2i|wis %-2i", user.str, user.stm, user.wis);
			display_string_xy(buffer, 0, 2 * 8);

			display.foreground = RED;
			sprintf(buffer, "%-10s = lvl %i", npc_names[id], lvl);
			display_string_xy(buffer, 0, 4 * 8);
			sprintf(buffer, "Life %-3i | Mana %-3i", life, mana);
			display_string_xy(buffer, 0, 5 * 8);

			display.foreground = WHITE;

			for (i = 0; i < 5; ++i)
			{
				sprintf(buffer, ":: %-25s", last_turn[i]);
				display_string_xy(buffer, 0, (9+i) * 8);
			}

			display.foreground = WHITE;
			sprintf(buffer, "pick your attack: DMG MAN");
			display_string_xy(buffer, 0, 15 * 8);

			
			uint8_t ii;
			for (ii = 0; ii < 5; ++ii)
			{
				if (selected_att == ii)
				{
					display.foreground = GREEN;
				}
				else
				{
					display.foreground = WHITE;
				}
				if (user.attacks[ii] != -1)
				{
					sprintf(buffer, " %-10s       %-3d %-3d", attack_names[user.attacks[ii]], _rb(&attacks[user.attacks[ii]][0]), _rb(&attacks[user.attacks[ii]][1]));
					display_string_xy(buffer, 0, (16 + ii) * 8);
				}
				
			}

			if (selected_att == 5)
			{
				display.foreground = GREEN;
			}
			else
			{
				display.foreground = WHITE;
			}
			sprintf(buffer, "%-8s", "exit");
			display_string_xy(buffer, 0, 22 * 8);

			redraw = 0;
		}

		if (user_turn)
		{
			if (right_pressed())
			{
				selected_att++;
				selected_att %= 6;
				redraw = 1;
			}
			else
			if (left_pressed())
			{
				selected_att--;
				selected_att = selected_att < 0 ? 5 : selected_att;
				redraw = 1;
			}else
			if (down_pressed())
			{
				redraw = 1;
			}else
			if (up_pressed())
			{
				redraw = 1;
			}else
			if (center_pressed())
			{
				if (selected_att == 5)
					break;
				
				// if selected att is available and with in mana
				if (user.attacks[selected_att] != -1)
				{
					att = user.attacks[selected_att];
					if((_rb(&attacks[att][3]) & 0xf) <= user.lvl)
					{
						if (user.mana >= _rb(&attacks[att][1]))
						{
							
							uint8_t m = _geo(missed(((combat_npcs[id][5] & 0xf0) >> 4), user.lvl));
							uint8_t att_details = _rb(&attacks[att][4]);
							uint8_t att_damage = _rb(&attacks[att][0]);

							for (i = 0; i < 4; ++i)
								sprintf(last_turn[i], "%s", last_turn[i + 1]);
							sprintf(last_turn[4], "%-10s %-6s %-10s", user.name, (m ? "missed" : "used"), attack_names[selected_att]);

							if ((_rb(&attacks[att][2]) >> 4) <= user.str &&
								(_rb(&attacks[att][2]) & 0xf) <= user.stm &&
								(_rb(&attacks[att][3]) >> 4) <= user.wis)
							{
								// if is attack
								if ((att_details & 0x1) == 0x1)
								{

									// monster new life
									new_l = life - att_damage;
									new_l = m ? life : new_l;
									// if monster dies
									if (new_l <= 0 && !m)
									{
										end_battle(id);
										break;
									}
									else // damage the monster
									{
										life = new_l;
									}

								}
								// if heal
								else if ((att_details & 0x2) == 0x2)
								{

									user.life += att_damage;
									if (user.life > user.max_life)
										user.life = user.max_life;

								}
							}
							else
							{
								for (i = 0; i < 4; ++i)
									sprintf(last_turn[i], "%s", last_turn[i + 1]);
								sprintf(last_turn[4], "not high enough stats !      ");
							}

							// if buff
							if ((att_details & 032) == 0x3)
							{
								if (last_att != -1)
								{
									user.str -= _rb(&attacks[last_att][2]) >> 4;
									user.stm -= _rb(&attacks[last_att][2]) & 0xf;
									user.wis -= _rb(&attacks[last_att][3]) >> 4;
								}
								user.str += _rb(&attacks[att][2]) >> 4;
								user.stm += _rb(&attacks[att][2]) & 0xf;
								user.wis += _rb(&attacks[att][3]) >> 4;
								buff_time = _rb(&attacks[att][3]) & 0xf;
								buff_timer = 0;
								last_att = user.attacks[selected_att];
							}

							// user uses mana
							user.mana -= _rb(&attacks[user.attacks[selected_att]][1]);

							// successful attack changes turn, otherwise, will refresh and loop round							

							buff_timer++;
							if (buff_timer == buff_time && last_att != -1)
							{
								user.str -= _rb(&attacks[last_att][2]) >> 4;
								user.stm -= _rb(&attacks[last_att][2]) & 0xf;
								user.wis -= _rb(&attacks[last_att][3]) >> 4;
								buff_timer = 0;
								last_att = -1;
							}

							user_turn = 0;
						}
						else
						{
							for (i = 0; i < 4; ++i)
								sprintf(last_turn[i], "%s", last_turn[i + 1]);
							sprintf(last_turn[4], "not enough mana !            ");
						}
					}
					else
					{
						for (i = 0; i < 4; ++i)
							sprintf(last_turn[i], "%s", last_turn[i + 1]);
						sprintf(last_turn[4], "not high enough level !      ");
					}
				}
				redraw = 1;
			}
		}
		else
		{
			
			// get random att from 2
			uint8_t
				att = _rand() % 2 ? combat_npcs[id][2] >> 4 : combat_npcs[id][2] & 0xf;				
			new_l2 = 0;
			// if att is within mana		
			if (mana < _rb(&attacks[att][1]))
				att = 0;		
			
			uint8_t m = _geo(missed(user.lvl, ((combat_npcs[id][5] & 0xf0) >> 4)));
			if (!m)
			{
				uint8_t att_details = _rb(&attacks[att][4]);
				uint8_t att_damage = _rb(&attacks[att][0]);

				// if is attack
				if ((att_details & 0x1) == 0x1)
				{
					new_l2 = user.life - att_damage;
					new_l2 = m ? user.life : new_l2;
					
					if (new_l2 <= 0 && !m)
					{
						death();
						break;
					}
					else
					{
						user.life = new_l2;
					}
				}
				// if heal
				else if ((att_details & 0x2) == 0x2)
				{
					life += att_damage;
					if (life > combat_npcs[id][3])
						life = combat_npcs[id][3];
				}				
			}

			mana -= _rb(&attacks[att][1]);

			for (i = 0; i < 4; ++i)
				sprintf(last_turn[i], "%s", last_turn[i + 1]);
			sprintf(last_turn[4], "%-10s %-6s %-10s", npc_names[id], (m ? "missed" : "used"), attack_names[att]);
			
			user_turn = 1;
			redraw = 1;
		}
	}

	draw_area(user.area_x, user.area_y);
	draw_gui(1);
}

int loop()
{
	uint8_t event_happened = 0, area_change = 0, speed = 0, id, blocks, blocked = 0,x, y, ax, ay;
	int8_t d, md;
	uint16_t r = 0;
	while (1) 
	{
		_delay_ms(30);
		r++;
		r %= 0xffff;
		blocked = 0;
		if (down_pressed())
		{
			store_mob_pos();
			user.x--;
			user.dir = DIR_L;
			event_happened = 1;
		}
		if (up_pressed())
		{
			store_mob_pos();
			user.x++;
			user.dir = DIR_R;
			event_happened = 1;
		}
		if (left_pressed())
		{
			store_mob_pos();
			user.y--;
			user.dir = DIR_B;
			event_happened = 1;
		}
		if (right_pressed())
		{
			store_mob_pos();
			user.y++;
			user.dir = DIR_F;
			event_happened = 1;
		}
		if (center_pressed())
		{
			draw_inventory();
		}
		if (event_happened)
		{		
			_srand(r);
			major_counter++;
			major_counter %= 0xffff;
			if(!(speed % mob_speed))
				for (id = 0; id < COMBAT_NPC_AMT; ++id)
				{
					if ((combat_npcs[id][5] & 0x4) == 0x4)
					{
						ax = (_rb(&npcs[id][0]) & 0xf0) >> 4;
						ay = (_rb(&npcs[id][0]) & 0xf);
						if (ax == user.area_x && ay == user.area_y)
						{
							x = combat_npcs[id][0] & 0xf0;
							x >>= 4;
							y = combat_npcs[id][0] & 0xf;
							combat_npcs[id][1] ^= combat_npcs[id][1];
							combat_npcs[id][1] |= combat_npcs[id][0];

							d = user.old_x - x;
							md = abs(d);
							if (d > 0)
							{
								combat_npcs[id][0] ^= combat_npcs[id][0] & 0xf0;
								combat_npcs[id][0] |= (x + 1) << 4;
								combat_npcs[id][5] ^= combat_npcs[id][5] & 0x3;
								combat_npcs[id][5] |= DIR_R;
							}
							else
								if (d < 0)
								{
									combat_npcs[id][0] ^= combat_npcs[id][0] & 0xf0;
									combat_npcs[id][0] |= (x - 1) << 4;
									combat_npcs[id][5] ^= combat_npcs[id][5] & 0x3;
									combat_npcs[id][5] |= DIR_L;
								}

							blocked = 0;
							for (blocks = 0; blocks < BLOCKED_TILES_AMT; ++blocks)
								blocked |=
								blocked_tiles[blocks][0] == _rb(&npcs[id][0]) &&
								blocked_tiles[blocks][1] == combat_npcs[id][0] &&
								(blocked_tiles[blocks][2] & 0x1) == 0x1;
							if (blocked)
							{
								combat_npcs[id][0] ^= combat_npcs[id][0] & 0xf0;
								combat_npcs[id][0] |= combat_npcs[id][1] & 0xf0;
							}

							d = user.old_y - y;
							md += abs(d);
							if (d > 0)
							{
								combat_npcs[id][0] ^= combat_npcs[id][0] & 0xf;
								combat_npcs[id][0] |= (y + 1) & 0xf;
								combat_npcs[id][5] ^= combat_npcs[id][5] & 0x3;
								combat_npcs[id][5] |= DIR_F;
							}
							else
								if (d < 0)
								{
									combat_npcs[id][0] ^= combat_npcs[id][0] & 0xf;
									combat_npcs[id][0] |= (y - 1) & 0xf;
									combat_npcs[id][5] ^= combat_npcs[id][5] & 0x3;
									combat_npcs[id][5] |= DIR_B;
								}

							blocked = 0;
							for (blocks = 0; blocks < BLOCKED_TILES_AMT; ++blocks)
								blocked |=
								blocked_tiles[blocks][0] == _rb(&npcs[id][0]) &&
								blocked_tiles[blocks][1] == combat_npcs[id][0] &&
								(blocked_tiles[blocks][2] & 0x1) == 0x1;
							if (blocked)
							{
								combat_npcs[id][0] ^= combat_npcs[id][0] & 0xf;
								combat_npcs[id][0] |= combat_npcs[id][1] & 0xf;
							}

							if (combat_npcs[id][0] != combat_npcs[id][1])
								redraw_tile(user.area_x, user.area_y, (combat_npcs[id][1] & 0xf0) >> 4, combat_npcs[id][1] & 0xf);
							redraw_tile(user.area_x, user.area_y, (combat_npcs[id][0] & 0xf0) >> 4, combat_npcs[id][0] & 0xf);

						}
					}
				}
			speed = (speed + 1) % mob_speed;

			if (user.x > 15)
			{
				user.x = 0;
				user.area_x++;
				area_change = 1;
			}
			if (user.x < 0)
			{
				user.x = 15;
				user.area_x--;
				area_change = 1;
			}
			if (user.y > 10)
			{
				user.y = 0;
				user.area_y++;
				area_change = 1;
			}
			if (user.y < 0)
			{
				user.y = 10;
				user.area_y--;
				area_change = 1;
			}
			
			
			blocked = 0;
			for (id = 0; id < BLOCKED_TILES_AMT; ++id)
				blocked |=
				(blocked_tiles[id][0] & 0xf0) >> 4 == user.area_x &&
				(blocked_tiles[id][0] & 0xf) == user.area_y &&
				(blocked_tiles[id][1] & 0xf0) >> 4 == user.x &&
				(blocked_tiles[id][1] & 0xf) == user.y &&
				(blocked_tiles[id][2] & 0x1) == 0x1;
			if (blocked)
			{
				user.x = user.old_x;
				user.y = user.old_y;
			}
			else
			{

				if (!(major_counter % 15))
				{
					user.life++;
					if (user.life > user.max_life)
						user.life = user.max_life;
				}
				if (!(major_counter % 10))
				{
					user.mana++;
					if (user.mana > user.max_mana)
						user.mana = user.max_mana;
				}

				uint8_t portal_id = 0;
				for (id = 0; id < PORTOL_AMT; ++id)
					if ((portals[id][2] & 0x80) == 0x80)
					{
						ax = (portals[id][0] & 0xf0) >> 4;
						ay = (portals[id][0] & 0xf);
						if (ax == user.area_x && ay == user.area_y)
							if ((portals[id][1] >> 4) == user.x && (portals[id][1] & 0xf) == user.y)
							{
								portal_id = portals[id][2] & 0x3f;
								user.area_x = portals[portal_id][0] >> 4;
								user.area_y = portals[portal_id][0] & 0xf;
								user.x = portals[portal_id][1] >> 4;
								user.y = portals[portal_id][1] & 0xf;
								if (portals[id][0] != portals[portal_id][0])
									area_change = 1;
								break;
							}
					}

				for (id = 0; id < COMBAT_NPC_AMT; ++id)
					if ((combat_npcs[id][5] & 0x4) == 0x4)
					{
						ax = (_rb(&npcs[id][0]) & 0xf0) >> 4;
						ay = (_rb(&npcs[id][0]) & 0xf);
						if (ax == user.area_x && ay == user.area_y)
							if (abs(user.x - ((combat_npcs[id][0] & 0xf0) >> 4)) + abs(user.y - (combat_npcs[id][0] & 0xf)) <= 1)
								draw_combat_screen(id);
					}

				redraw_user();
			}

				
			if (area_change)
			{
				draw_area(user.area_x, user.area_y);
				draw_gui(1);
				for (id = 0; id < COMBAT_NPC_AMT; ++id)
					combat_npcs[id][5] |= 0x4;
				area_change = 0;
			}

			//// other ////
			draw_gui(0);
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

	display.background = BLACK;

	buffer = malloc(s);	

	draw_area(user.area_x, user.area_y);
	draw_gui(1);
	//print_debug_text();
	loop();
	
	//uint8_t i;
	//for (i = 0; i < tiles; i++)
	//overlay_sprite6(0, i * tile_size, tile_size, tile_data[i]);

	/*
	for (i = 0; i < masks; i++)
	{
		mask_sprite6(1 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], 0);
		mask_sprite6(2 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i);
		mask_sprite6(3 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | VERT_FLIP);
		mask_sprite6(4 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | HORI_FLIP);
		mask_sprite6(5 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | VERT_FLIP | HORI_FLIP);
		mask_sprite6(6 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | HORI_FLIP | VERT_FLIP);
		mask_sprite6(7 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | VERT_FLIP | INVERT);
		mask_sprite6(8 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | HORI_FLIP | INVERT);
		mask_sprite6(9 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | VERT_FLIP | HORI_FLIP | INVERT);
		mask_sprite6(10 * tile_size, i * tile_size, tile_size, tile_data[water], mask_data[i], i | HORI_FLIP | VERT_FLIP | INVERT);
	}*/
	/*sprintf(buffer, "user Coord:     %d, %d", -1, -1);
	display_string_xy(buffer, 0, 0);
	sprintf(buffer, "Old user Coord: %d, %d", -1, -1);
	display_string_xy(buffer, 0, 8);
	sprintf(buffer, "Room Coord: %d, %d", -1, -1);
	display_string_xy(buffer, 0, 16);*/
	return 1;
}

