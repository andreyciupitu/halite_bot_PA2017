#include "hlt.hpp"
#include <iostream>

int get_nearest_border(hlt::Player p, hlt::Location l)
{
	int min = 1000;
	/* Directia pe care o returnez/ */
	int direction = STILL; 

	if(std::abs(p.topLeft.x - l.x) < std::abs(p.botRight.x - l.x))
	{
		min = std::abs(p.topLeft.x - l.x);
		direction = WEST;
	}
	else
	{
		min = std::abs(p.botRight.x - l.x);
		direction = EAST;
	}
	if(std::abs(p.topLeft.y - l.y) < min)
	{
		min = std::abs(p.topLeft.y - l.y);
		direction = NORTH;
	}
	if(std::abs(p.botRight.y - l.y) < min)
	{
		min = std::abs(p.botRight.y - l.y);
		direction = SOUTH;
	}
	return direction;
}
