#include "stdafx.h" //<--VISUAL STUDIO BULLSHIT
#include <algorithm>
#include <cmath>
#include <fstream>

#include "hlt.hpp"
#include "Player.h"

Player::Player(hlt::Location topLeft, hlt::Location botRight, unsigned char id)
{ 
	this->topLeft = topLeft;
	this->botRight = botRight;
	this->id = id;
}

int Player::get_nearest_border(hlt::GameMap &map, hlt::Location l)
{
	/* TARANEALA --------------
	
		NEAREST TILE NOT OWNED BY ME
	
	*/

	int min_distance = 1000; //initializare cu valoare mai buna
	int direction = STILL;

	for (int i = 0; i < 4; i++)
	{
		int distance = 0;
		hlt::Location current_tile = l;
		while (map.getSite(current_tile).owner == id && distance < min_distance)
		{
			distance++;
			current_tile = map.getLocation(current_tile, CARDINALS[i]);
		}
		if (distance < min_distance)
		{
			min_distance = distance;
			direction = CARDINALS[i];
		}
	}
	return direction;
}

hlt::Move Player::make_a_move(hlt::GameMap &map, hlt::Location l)
{
	hlt::Move result_move;

	/* CURRENT TILE */
	hlt::Site site = map.getSite(l);

	/* CHECK IF THE TILE SHOULD MOVE */
	if (site.strength < 5 * site.production)
	{
		result_move.loc = l;
		result_move.dir = STILL;
		return result_move;
	}

	if (isOnBorder(map, l))
	{
		/* ATTACK IF POSSIBLE */
		for (int i = 0; i < 4; i++)
		{
			if (map.getSite(l, CARDINALS[i]).owner != id &&
				map.getSite(l, CARDINALS[i]).strength < site.strength)
			{
				result_move.loc = l;
				result_move.dir = CARDINALS[i];
				topLeft.x = std::min(topLeft.x, map.getLocation(l, CARDINALS[i]).x);
				topLeft.y = std::min(topLeft.y, map.getLocation(l, CARDINALS[i]).y);
				botRight.x = std::max(botRight.x, map.getLocation(l, CARDINALS[i]).x);
				botRight.y = std::max(botRight.y, map.getLocation(l, CARDINALS[i]).y);
				return result_move;
			}
		}

		/* CAN'T ATTACK => STILL*/
		result_move.loc = l;
		result_move.dir = STILL;
		return result_move;
	}

	/* SEND THE TILE TO A BORDER */
	switch (get_nearest_border(map, l))
	{
	case NORTH:
		result_move.loc = l;
		result_move.dir = NORTH;
		return result_move;
	case EAST:
		result_move.loc = l;
		result_move.dir = EAST;
		return result_move;
	case SOUTH:
		result_move.loc = l;
		result_move.dir = SOUTH;
		return result_move;
	case WEST:
		result_move.loc = l;
		result_move.dir = WEST;
		return result_move;
	default:
		result_move.loc = l;
		result_move.dir = STILL;
		return result_move;
	}
}

bool Player::isOnBorder(hlt::GameMap &map, hlt::Location l)
{
	for (int i = 0; i < 4; i++)
		if (map.getSite(l, CARDINALS[i]).owner != id)
			return true;
	return false;
}

bool Player::isInside(hlt::Location l)
{
	return (topLeft.x <= l.x && topLeft.y <= l.y && l.x <= botRight.x && l.y <= botRight.y);
}


