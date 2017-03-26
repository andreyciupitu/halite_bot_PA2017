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
	Player p(presentMap.width, presentMap.height, myID);

	/* DO MORE STUFF HERE
		.....
		.....
	 */

	/* START GAME */
    sendInit("202v1.2.2");

    std::set<hlt::Move> moves;
    while(true)
	{

		/* RESET MOVES */
        moves.clear();

        getFrame(presentMap);

		/* Get current strength map */
        for(unsigned short i = 0; i < presentMap.width; i++)
			for(unsigned short j = 0; j < presentMap.height; j++)
				p.strengthMap[i][j] = presentMap.getSite({ i, j }).strength;

		/* Assign a move for each tile */
        for(unsigned short i = 0; i < presentMap.width; i++)
			for(unsigned short j = 0; j < presentMap.height; j++)
				if (presentMap.getSite({ i, j }).owner == myID)
					moves.insert(p.make_a_move(presentMap, { i, j }));

		/* END TURN */
        sendFrame(moves);
    }

    return 0;
}
