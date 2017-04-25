#include <algorithm>
#include "hlt.hpp"

#include "Player.hpp"

#define WAIT_TIME 5

Player::Player(int width, int height, unsigned char id)
{
	this->id = id;
	strengthMap = std::vector< std::vector<int> >(width, std::vector<int>(height, 0));
	directionMap = std::vector< std::vector<int> >(width, std::vector<int>(height, 0));
}

int Player::get_nearest_border(hlt::GameMap &map, hlt::Location l)
{
	/* TARANEALA --------------

		NEAREST TILE NOT OWNED BY ME

	*/

	int min_distance = map.height / 2;
	int direction = SOUTH;

	// TODO add heuristic here too
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
		updateStrengthMap(map, l, STILL);
		result_move.dir = STILL;
		return result_move;
	}

	/* IF THE TILE IS ON BORDER */
	if (isOnBorder(map, l))
	{
		/* ATTACK IF POSSIBLE CHOOSING THE BEST TILE */
		double maxScore = 0;
		int bestMove = STILL;
		for (int i = 0; i < 4; i++)
		{
			hlt::Site nextSite = map.getSite(l, CARDINALS[i]);
			if (nextSite.owner != id && nextSite.strength < site.strength)
			{
				double score = evaluate(map, map.getLocation(l, CARDINALS[i]));
				if (score >= maxScore)
				{
					maxScore = score;
					bestMove = CARDINALS[i];
				}
			}
		}

		/* CAN'T ATTACK => STILL*/
		result_move.dir = bestMove;

		/* Check if it's better to combine
		with a neighbouring tile */
		if (bestMove == STILL)
			for (int i = 0; i < 4; i++)
			{
				int maxStrength = strengthMap[l.x][l.y] + site.production;
				hlt::Location neighbour = map.getLocation(l, CARDINALS[i]);
				if (map.getSite(neighbour).owner == id && isOnBorder(map, neighbour))
				{
					if (site.strength + map.getSite(neighbour).strength > maxStrength
						&& site.strength + map.getSite(neighbour).strength <= 255)
					{
						maxStrength = site.strength + strengthMap[neighbour.x][neighbour.y];
						result_move.dir = CARDINALS[i];
					}
				}
			}

		updateStrengthMap(map, l, result_move.dir);
		return result_move;
	}

	/* SEND THE TILE TO A BORDER */
	result_move.dir = get_nearest_border(map, l);

	// SULITA LU' NICU UNCOMMENT 4 VICTORY
/*	int dist = 0;
	hlt::Location aux = l;
	while (dist < 6)
	{
		if(map.getSite(aux,result_move.dir).owner != id && map.getSite(aux,result_move.dir).owner != 0)
		{
			break;
		}
		aux = map.getLocation(aux,result_move.dir);
		dist ++;
	}

	if  (dist == 5 && map.getSite(l).strength < 255)
	{
		result_move.dir = STILL;
	}
*/
	updateStrengthMap(map, l, result_move.dir);
	return result_move;
}

bool Player::isOnBorder(hlt::GameMap &map, hlt::Location l)
{
	for (int i = 0; i < 4; i++)
		if (map.getSite(l, CARDINALS[i]).owner != id)
			return true;
	return false;
}

int Player::canSaveStrength(hlt::GameMap &map)
{
	int count = 0;
	for (unsigned short i = 0; i < map.width; i++)
		for (unsigned short j = 0; j < map.height; j++)
			if (strengthMap[i][j] > 255)
			{
				count++;
				if (directionMap[i][j] == STILL)
					for (int k = 0; k < 4; k++)
					{
						hlt::Location neighbour = map.getLocation({i, j}, CARDINALS[k]);
						if (directionMap[neighbour.x][neighbour.y] == (CARDINALS[k] + 2) % 4)
						{
							updateStrengthMap(map, {i, j}, CARDINALS[k]);
							directionMap[i][j] = CARDINALS[k];
							break;
						}
					}
				else//if (strengthMap[i][j] > 255)
					for (int k = 0; k < 4; k++)
					{
						hlt::Location neighbour = map.getLocation({i, j}, CARDINALS[k]);
						if (directionMap[neighbour.x][neighbour.y] == ((CARDINALS[k] + 2) % 4)
							&& map.getSite(neighbour).owner == id)
						{
							int right =  (CARDINALS[k] + 3) % 4;
							int left = (CARDINALS[k] + 1) % 4;
							hlt::Location rightChoice = map.getLocation(neighbour, right);
							hlt::Location leftChoice = map.getLocation(neighbour, left);
							int move_right = map.getSite(neighbour).strength + strengthMap[rightChoice.x][rightChoice.y];
							int move_left = map.getSite(neighbour).strength + strengthMap[leftChoice.x][leftChoice.y];
							if (move_left < move_right && move_left < strengthMap[i][j])
							{
								updateStrengthMap(map, neighbour, left);
								directionMap[neighbour.x][neighbour.y] = left;
							}
							else
								if (move_right < strengthMap[i][j])
								{
									updateStrengthMap(map, neighbour, right);
									directionMap[neighbour.x][neighbour.y] = right;
								}
								else
									continue;
							break;
						}
					}
			}
	return count;
}

void Player::updateStrengthMap(hlt::GameMap &map, hlt::Location l, int direction)
{
	hlt::Site site = map.getSite(l);

	/* STILL => INCREASE BY PRODUCTION */
	if (direction == STILL)
		strengthMap[l.x][l.y] += site.production;
	else
	{
		strengthMap[l.x][l.y] -= site.strength;
		hlt::Location neighbour = map.getLocation(l, direction);
		strengthMap[neighbour.x][neighbour.y] += site.strength;
	}
}

/* EXPANSION HEURISTIC */
double Player::evaluate(hlt::GameMap &map, hlt::Location l)
{
	hlt::Site site = map.getSite(l);
	if (site.owner == 0 && site.strength > 0)
		return site.production / (double)site.strength;
	else
	{
		int damageTaken = 0;
		for (int i = 0; i < 4; i++)
		{
			hlt::Site neighbour = map.getSite(l, i);
			if (neighbour.owner != site.owner)
				damageTaken += neighbour.strength;
		}
		return damageTaken;
	}
}
