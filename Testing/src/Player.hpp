#ifndef PLAYER_H
#define PLAYER_H

#include "hlt.hpp"

class Player
{
public:
	std::vector< std::vector<int> > strengthMap;
	std::vector< std::vector<int> > directionMap;
	unsigned char id;

	Player(int width, int height, unsigned char id);
	hlt::Move make_a_move(hlt::GameMap &map, hlt::Location l);
	int canSaveStrength(hlt::GameMap &map);

private:
	int get_nearest_border(hlt::GameMap &map, hlt::Location l);
	bool isOnBorder(hlt::GameMap &map, hlt::Location l);
	void updateStrengthMap(hlt::GameMap &map, hlt::Location l, int direction);
	double evaluate(hlt::GameMap &map, hlt:: Location l);
};

#endif // !PLAYER_H
