#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"
#include "Player.hpp"

int main()
{

    std::cout.sync_with_stdio(0);

	/* ERROR/DEBUG LOG FILE */
	std::ofstream fout("output_log.txt");
	fout.close();

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);

	/* INITIALIZE PLAYER HERE */
	Player bot(presentMap.width, presentMap.height, myID);

	/* DO MORE STUFF HERE
		.....
		.....
	 */

	/* START GAME */
    sendInit("202v2");

    std::set<hlt::Move> moves;
    while(true)
	{

		/* RESET MOVES */
        moves.clear();

        getFrame(presentMap);

		/* Get current strength map */
        for(unsigned short i = 0; i < presentMap.width; i++)
			for(unsigned short j = 0; j < presentMap.height; j++)
			{
                hlt::Site site = presentMap.getSite({ i, j });
                if (site.owner == myID)
                {
                    bot.directionMap[i][j] = STILL;
                    bot.strengthMap[i][j] = site.strength;
                }
                else
                {
                    bot.strengthMap[i][j] = -1 * site.strength;
                    bot.directionMap[i][j] = -1;
                }
            }
		/* Assign a move for each tile */
        for(unsigned short i = 0; i < presentMap.width; i++)
			for(unsigned short j = 0; j < presentMap.height; j++)
				if (presentMap.getSite({ i, j }).owner == myID)
				{
                    hlt::Move move = bot.make_a_move(presentMap, {i, j});
                    bot.directionMap[i][j] = move.dir;
                }
                
        int last_turn = presentMap.height * presentMap.width;
        int fixxed = bot.canSaveStrength(presentMap);
        while (last_turn > fixxed)
        {
            last_turn = fixxed;
            fixxed = bot.canSaveStrength(presentMap);
        }

        for(unsigned short i = 0; i < presentMap.width; i++)
            for(unsigned short j = 0; j < presentMap.height; j++)
                if (presentMap.getSite({ i, j }).owner == myID)
                {
                    hlt::Move move;
                    move.dir = bot.directionMap[i][j];
                    move.loc.x = i;
                    move.loc.y = j;
                    moves.insert(move);
                }

		/* END TURN */
        sendFrame(moves);
    }

    return 0;
}
