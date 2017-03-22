//#include "stdafx.h"  //<-VISUAL STUDIO BULLSHIT
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <time.h>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"
#include "Player.h"

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
	
	hlt::Location l;
	for (unsigned short a = 0; a < presentMap.height; a++)
	{
		for (unsigned short b = 0; b < presentMap.width; b++)
		{
			if (presentMap.getSite({ b, a }).owner == myID)
			{
				l = { b, a };
			}
		}
	}

	/* DO MORE STUFF HERE */
	
	Player p(l, l, myID);

	/* START */
    sendInit("202v1.2");

    std::set<hlt::Move> moves;
    while(true)
	{

		/* RESET MOVES */
        moves.clear();

        getFrame(presentMap);

        for(unsigned short i = 0; i < presentMap.width; i++)
			for(unsigned short j = 0; j < presentMap.height; j++)
				p.strengthMap[i][j] = presentMap.getSite({ i, j }).strength;
        for(unsigned short i = 0; i < presentMap.width; i++)
			for(unsigned short j = 0; j < presentMap.height; j++)
			{
				if (presentMap.getSite({ i, j }).owner == myID)
					moves.insert(p.make_a_move(presentMap, { i, j }));
            }

		/* END TURN */
        sendFrame(moves);
    }

    return 0;
}
