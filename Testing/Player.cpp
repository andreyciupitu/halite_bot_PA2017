//#include "stdafx.h" //<--VISUAL STUDIO BULLSHIT
#include <algorithm>
#include <cmath>
#include <fstream>

#include "hlt.hpp"
#include "Player.h"

#define WAIT_TIME 5

Player::Player(hlt::Location topLeft, hlt::Location botRight, unsigned char id)
{ 
	this->topLeft = topLeft;
	this->botRight = botRight;
	this->id = id;
	strengthMap = std::vector<std::vector<unsigned char>>(50, std::vector<unsigned char>(50, 0));
}

int Player::get_nearest_border(hlt::GameMap &map, hlt::Location l)
{
	/* TARANEALA --------------
	
		NEAREST TILE NOT OWNED BY ME
	
	*/

	int min_distance = map.height / 2;
	int direction = SOUTH;

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
	result_move.loc = l;

	/* CURRENT TILE */
	hlt::Site site = map.getSite(l);

	/* CHECK IF THE TILE SHOULD MOVE */
	if (site.strength < WAIT_TIME * site.production || site.strength == 0)
	{
		strengthMap[l.x][l.y] += site.production;
		result_move.dir = STILL;
		return result_move;
	}

	/* IF THE TILE IS ON BORDER */
	if (isOnBorder(map, l))
	{
		/* ATTACK IF POSSIBLE */
		double maxScore = 0;
		int bestMove = STILL;
		for (int i = 0; i < 4; i++)
		{
			hlt::Site nextSite = map.getSite(l, CARDINALS[i]);
			if (nextSite.owner != id && nextSite.strength < site.strength)
			{
				double score = evaluate(nextSite);
				if (score >= maxScore)
				{
					maxScore = score;
					bestMove = CARDINALS[i];
				}
			}
		}

		/* CAN'T ATTACK => STILL*/
		result_move.dir = bestMove;

		if (bestMove == STILL)
			for(int i = 0; i < 4; i++)
			{
				int maxStrength = strengthMap[l.x][l.y] + site.production;
				hlt::Location neighbour = map.getLocation(l, CARDINALS[i]);
				if(map.getSite(neighbour).owner == id && isOnBorder(map, neighbour))
				{
					if(site.strength + strengthMap[neighbour.x][neighbour.y] > maxStrength && site.strength + strengthMap[neighbour.x][neighbour.y] <= 255)
					{
						maxStrength = site.strength + strengthMap[neighbour.x][neighbour.y];
						result_move.dir = CARDINALS[i];
					}
				}
			}

		strengthMap[l.x][l.y] = 0;
		hlt::Location neighbour = map.getLocation(l, result_move.dir);
		strengthMap[neighbour.x][neighbour.y] += site.strength;
		return result_move;
	}

	/* SEND THE TILE TO A BORDER */
	result_move.dir = get_nearest_border(map, l);
	strengthMap[l.x][l.y] = 0;
	hlt::Location neighbour = map.getLocation(l, result_move.dir);
	strengthMap[neighbour.x][neighbour.y] += site.strength;
	return result_move;
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

double evaluate(hlt::Site site)
{
	return site.strength == 0 ? site.production : site.production/(double)site.strength;
}