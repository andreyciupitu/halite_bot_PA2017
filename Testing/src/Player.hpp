#ifndef PLAYER_H
#define PLAYER_H

#include <set>

#include "hlt.hpp"

#define WAIT_TIME 5

class Player
{
public:
	hlt::GameMap &map;
	unsigned char id;

	Player(hlt::GameMap &map, unsigned char id);

	/**
	 * Chooses the best move for each tile.
	 * This functios calls make_a_move for each given tile and updates the
	 * direction map.
	 *
	 * @method assignMoves
	 */
	void assignMoves();

	/**
	 * Initializes the internal maps from the current map state. Initially all
	 * of the bot's own tiles will be considered STILL.
	 *
	 * @method getMapDetails
	 */
	void getMapDetails();

	/**
	 * Attempt to correct the strength capping by redirecting inbound tiles
	 * and moving away STILL tiles.
	 *
	 * @method canSaveStrength
	 *
	 * @return The number of overcapped tiles found.
	 */
	int canSaveStrength();

	/**
	 * Makes a set of Moves from the direction map.
	 *
	 * @method getMoveSet
	 *
	 * @return A set of the assigned moves.
	 */
	std::set<hlt::Move> getMoveSet();

private:
	std::vector< std::vector<int> > strengthMap;
	std::vector< std::vector<int> > directionMap;

	/**
	 * Main funtion used to assign a move to a given tile.
	 *
	 * @method make_a_move
	 *
	 * @param  l           Tile location.
	 *
	 * @return The direction of the move.
	 */
	int make_a_move(hlt::Location l);

	/**
	 * Finds the nearest tile not owned by me, by checking in all directions
	 * in a straigth line, starting from the specified location.
	 *
	 * @method get_nearest_border
	 *
	 * @param  l                  Starting tile location.
	 *
	 * @return The direction to the nearest enemy tile.
	 */
	int get_nearest_border(hlt::Location l);

	/**
	 * Checks whether the given tile is set on the border of the player's
	 * terrtory.
	 *
	 * @method isOnBorder
	 *
	 * @param  l          Tile location.
	 *
	 * @return TRUE if the tile is on the border.
	 */
	bool isOnBorder(hlt::Location l);

	/**
	 * Updates the internal strength map with the given move.
	 *
	 * @method updateStrengthMap
	 *
	 * @param  l                 Current tile position.
	 * @param  direction         Move direction.
	 */
	void updateStrengthMap(hlt::Location l, int direction);

	/**
	 * Main heuristic function, that dictates the logic of the bot. It chooses
	 * the best NPC tile (the tile with the best value), or the weakest enemy
	 * tile (the tile that suffers the most damage).
	 *
	 * @method evaluate
	 *
	 * @param  l        The location to be evaluated.
	 *
	 * @return The score given to the tile.
	 */
	double evaluate(hlt:: Location l);
};

#endif // !PLAYER_H
