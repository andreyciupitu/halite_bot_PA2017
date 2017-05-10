#include <algorithm>
#include <queue>
#include <utility>
#include "hlt.hpp"

#include "Player.hpp"

Player::Player(hlt::GameMap &map, unsigned char id) : map(map)
{
	this->id = id;
	strengthMap = std::vector< std::vector<int> >(map.width, std::vector<int>(map.height, 0));
	directionMap = std::vector< std::vector<int> >(map.width, std::vector<int>(map.height, 0));
	earlyGame = true;
	scoreMap = std::vector< std::vector<double> >(map.width, std::vector<double>(map.height, -1));
}

int Player::get_nearest_border(hlt::Location l)
{
	int min_distance = map.height / 2;
	int direction = SOUTH;
	double bestScore = 0;

	if (earlyGame)
		for (int i = 0; i < 4; i++)
		{
			int distance = 0;
			hlt::Location current_tile = l;

			/* STOP AT THE FIRST ENEMY TILE */
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
	else
		for (int i = 0; i < 4; i++)
		{
			hlt::Location neighbour = map.getLocation(l, CARDINALS[i]);
			if (scoreMap[neighbour.x][neighbour.y] > bestScore)
			{
				bestScore = scoreMap[neighbour.x][neighbour.y];
				direction = CARDINALS[i];
			}
		}
	return direction;
}

int Player::make_a_move(hlt::Location l)
{
	int result_move = STILL;

	/* CURRENT TILE */
	hlt::Site site = map.getSite(l);

	/* CHECK IF THE TILE SHOULD MOVE */
	if (site.strength < WAIT_TIME * site.production || site.strength == 0)
	{
		updateStrengthMap(l, STILL);
		result_move = STILL;
		return result_move;
	}

	/* IF THE TILE IS ON BORDER */
	if (isOnBorder(l))
	{
		/* ATTACK IF POSSIBLE CHOOSING THE BEST TILE */
		double maxScore = 0;
		int bestMove = STILL;
		for (int i = 0; i < 4; i++)
		{
			hlt::Site nextSite = map.getSite(l, CARDINALS[i]);
			if (nextSite.owner != id && nextSite.strength < site.strength)
			{
				double score = borderEvaluate(map.getLocation(l, CARDINALS[i]));
				if (score >= maxScore)
				{
					maxScore = score;
					bestMove = CARDINALS[i];
				}
			}
		}

		/* CAN'T ATTACK => STILL*/
		result_move = bestMove;

		/* Check if it's better to combine with a neighbouring tile */
		if (bestMove == STILL)
			for (int i = 0; i < 4; i++)
			{
				int maxStrength = strengthMap[l.x][l.y] + site.production;
				hlt::Location neighbour = map.getLocation(l, CARDINALS[i]);
				int result = site.strength + strengthMap[neighbour.x][neighbour.y];
				if (map.getSite(neighbour).owner == id && isOnBorder(neighbour) && result > maxStrength)
				{
					maxStrength = result;
					result_move = CARDINALS[i];
				}
			}

		updateStrengthMap(l, result_move);
		return result_move;
	}

	/* SEND THE TILE TO A BORDER */
	result_move = get_nearest_border(l);
	updateStrengthMap(l, result_move);
	return result_move;
}

bool Player::isOnBorder(hlt::Location l)
{
	for (int i = 0; i < 4; i++)
		if (map.getSite(l, CARDINALS[i]).owner != id)
			return true;
	return false;
}

void Player::assignMoves()
{
	for(unsigned short i = 0; i < map.width; i++)
		for(unsigned short j = 0; j < map.height; j++)
			if (map.getSite({ i, j }).owner == id)
				directionMap[i][j] = make_a_move({i, j});
}

std::set<hlt::Move> Player::getMoveSet()
{
	std::set<hlt::Move> moves;
	for(unsigned short i = 0; i < map.width; i++)
		for(unsigned short j = 0; j < map.height; j++)
			if (map.getSite({ i, j }).owner == id)
			{
				hlt::Move move;
				move.dir = directionMap[i][j];
				move.loc.x = i;
				move.loc.y = j;
				moves.insert(move);
			}
	return moves;
}

void Player::getMapDetails()
{
	std::priority_queue< std::pair<double, hlt::Location> > q;
	for(unsigned short i = 0; i < map.width; i++)
		for(unsigned short j = 0; j < map.height; j++)
		{
			hlt::Site site = map.getSite({ i, j });
			scoreMap[i][j] = -1;
			if (site.owner == id)
			{
				directionMap[i][j] = STILL;
				strengthMap[i][j] = site.strength;
			}
			else
			{
				hlt::Location current = { i, j };
				q.push(std::pair<double, hlt::Location>(evaluate(current), current));

				/* Enemy tiles don't have any assigned moves,
				and negative initial score for easier mapping */
				strengthMap[i][j] = -1 * site.strength;
				directionMap[i][j] = -1;
			}
		}
	int count = 0;
	while (count < map.height * map.width)
	{
		std::pair<double, hlt::Location> current = q.top();
		hlt::Location tile = current.second;
		q.pop();
		if (scoreMap[tile.x][tile.y] != -1)
			continue;
		count++;
		scoreMap[tile.x][tile.y] = current.first;
		for (int i = 0; i < 4; i++)
		{
			hlt::Location next = map.getLocation(tile, CARDINALS[i]);
			double alpha = 0.1;
			if (map.getSite(next).owner == id)
				alpha = 0.5;
		 	double score = (1 - alpha) * scoreMap[tile.x][tile.y] + alpha * evaluate(next);
			q.push(std::pair<double, hlt::Location>(score, next));
		}
	}
}

int Player::canSaveStrength()
{
	int count = 0;
	for (unsigned short i = 0; i < map.width; i++)
		for (unsigned short j = 0; j < map.height; j++)
			if (strengthMap[i][j] > 255)
			{
				count++;

				/* MOVE THE TILE IF IT STAYS STILL
				(SWAPS IT WITH AN INBOUND TILE) */
				if (directionMap[i][j] == STILL && map.getSite({i, j}).strength > 0)
					for (int k = 0; k < 4; k++)
					{
						int back = (CARDINALS[k] + 2) % 4;
						hlt::Location neighbour = map.getLocation({i, j}, CARDINALS[k]);
						if (directionMap[neighbour.x][neighbour.y] == back)
						{
							updateStrengthMap({i, j}, CARDINALS[k]);
							directionMap[i][j] = CARDINALS[k];
							break;
						}
					}
				else
					for (int k = 0; k < 4; k++)
					{
						int back = (CARDINALS[k] + 2) % 4;
						int right = (CARDINALS[k] + 1) % 4;
						int left = (CARDINALS[k] + 3) % 4;
						hlt::Location neighbour = map.getLocation({i, j}, CARDINALS[k]);
						hlt::Site nextTile = map.getSite(neighbour);
						hlt::Location rightChoice = map.getLocation(neighbour, right);
						hlt::Location leftChoice = map.getLocation(neighbour, left);

						/* Attempt to redirect an inbound tile, either to the
						right, or to the left, minimizing the strenght loss
						(backwards is useless!!!) */
						if (directionMap[neighbour.x][neighbour.y] == back && nextTile.owner == id)
						{
							int move_right = nextTile.strength + strengthMap[rightChoice.x][rightChoice.y];
							int move_left = nextTile.strength + strengthMap[leftChoice.x][leftChoice.y];

							/* Choose the direction that saves the most strength,
							or try another tile if no strength can be saved */
							if (move_left < move_right && move_left < strengthMap[i][j])
							{
								updateStrengthMap(neighbour, left);
								directionMap[neighbour.x][neighbour.y] = left;
							}
							else
								if (move_right < strengthMap[i][j])
								{
									updateStrengthMap(neighbour, right);
									directionMap[neighbour.x][neighbour.y] = right;
								}
								else
									continue;

							/* BREAK IF SUCCESS */
							break;
						}
					}
			}
	return count;
}

void Player::updateStrengthMap(hlt::Location l, int direction)
{
	hlt::Site site = map.getSite(l);

	/* STILL => INCREASE BY PRODUCTION */
	if (direction == STILL)
		strengthMap[l.x][l.y] += site.production;
	else
	{
		/* CURRENT TILE MOVES AWAY */
		strengthMap[l.x][l.y] -= site.strength;

		/* COMBINES WITH DESTINATION */
		hlt::Location neighbour = map.getLocation(l, direction);
		strengthMap[neighbour.x][neighbour.y] += site.strength;
	}
}

double Player::evaluate(hlt::Location l)
{
	hlt::Site site = map.getSite(l);
	if (site.owner != id && site.strength > 0)
		return (double)site.strength / site.production;
	return -borderEvaluate(l);
}

double Player::borderEvaluate(hlt::Location l)
{
	hlt::Site site = map.getSite(l);

	/* NPC TILE */
	if (site.owner == 0 && site.strength > 0)
		return site.production / (double)site.strength;
	else
	{
		/* ENEMY TILE */
		int damageTaken = 0;
		for (int i = 0; i < 4; i++)
		{
			hlt::Site neighbour = map.getSite(l, CARDINALS[i]);
			if (neighbour.owner != site.owner && neighbour.owner != 0)
				damageTaken += 1;
		}
		return damageTaken;
	}

}
