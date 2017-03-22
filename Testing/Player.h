#ifndef PLAYER_H
#define PLAYER_H

#include "hlt.hpp"

class Player
{
public:
	hlt::Location topLeft;
	hlt::Location botRight;
	std::vector< std::vector<unsigned char> > strengthMap;
	unsigned char id;

	Player(hlt::Location topLeft, hlt::Location botRight, unsigned char id);
	hlt::Move make_a_move(hlt::GameMap &map, hlt::Location l);

private:
	int get_nearest_border(hlt::GameMap &map, hlt::Location l);
	bool isOnBorder(hlt::GameMap &map, hlt::Location l);
	bool isInside(hlt::Location l);
};

#endif // !PLAYER_H


