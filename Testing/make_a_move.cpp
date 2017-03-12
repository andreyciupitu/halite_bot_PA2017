#include "hlt.hpp"
using namespace hlt;
int get_nearest_border(Player p, Location l);
Move make_a_move(GameMap &map, Player p, Location l)
{
	Move result_move;
	Site site=map.getSite(l);
	if (site.strength < 5 * site.production)
	{
		result_move.loc=l;
		result_move.dir=STILL;
		return result_move;
	}
	if(isOnBorder(p,l))
	{
		for(int i=0;i<4;i++)
		{
			if(map.getSite(l,CARDINALS[i]).owner!=p.id && map.getSite(l,CARDINALS[i]).strength < site.strength)
			{
				result_move.loc=l;
				result_move.dir=CARDINALS[i];
				return result_move;
			}
		}
		result_move.loc=l;
		result_move.dir=STILL;
		return result_move;
	}

	switch(get_nearest_border(p,l))
	{
		case NORTH:
			result_move.loc=l;
			result_move.dir=NORTH;
			return result_move;
		case EAST:
			result_move.loc=l;
			result_move.dir=EAST;
			return result_move;
		case SOUTH:
			result_move.loc=l;
			result_move.dir=SOUTH;
			return result_move;
		case WEST:
			result_move.loc=l;
			result_move.dir=WEST;
			return result_move;
	}

}