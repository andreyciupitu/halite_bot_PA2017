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
	Player bot(presentMap, myID);

	/* START GAME */
    sendInit("202v3");

    std::set<hlt::Move> moves;
    int count = 0;
    while(true)
	{
		/* RESET MOVES */
        moves.clear();
        count++;
        if (count > 100)
            bot.earlyGame = false;
        getFrame(presentMap);

		/* GET MAP STRENGHT & REINITIALIZE DIRECTIONS */
        bot.getMapDetails();

		/* MOVE TILES */
        bot.assignMoves();

        /* CORRECT STRENGTH CAPPING */
        int capped = INF;
        int fixed = bot.canSaveStrength();

        /* Try until no further improvements can be made */
        while (capped > fixed)
        {
            capped = fixed;
            fixed = bot.canSaveStrength();
        }

        /* GET THE FINAL MOVES */
        moves = bot.getMoveSet();

		/* END TURN */
        sendFrame(moves);
    }

    return 0;
}
