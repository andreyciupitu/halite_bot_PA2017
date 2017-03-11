#include "stdafx.h" /* <----- CLICK DREAPTA DELET IN MORTI MATI DACA NU LUCREZI PE VISUAL STUDIO */
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

int main()
{
	/* RNG SEED */
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

	/* ERROR/DEBUG LOG FILE */
	std::ofstream fout("output_log.txt");
	fout.close();

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);

	/* INITIALIZE PLAYER HERE
	*
	*
	*	
	*
	*/

	/* START */
    sendInit("202");

    std::set<hlt::Move> moves;
    while(true)
	{

		/* RESET MOVES */
        moves.clear();

        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++)
		{
            for(unsigned short b = 0; b < presentMap.width; b++)
			{
                if (presentMap.getSite({ b, a }).owner == myID)
				{
					if (presentMap.getSite({ b, a }).strength < 6 * presentMap.getSite({ b, a }).production)
						moves.insert({ {b, a}, STILL });
					else
						moves.insert({ { b, a }, (unsigned char)(rand() % 2 + 1) });
                }
            }
        }

		/* END TURN */
        sendFrame(moves);
    }

    return 0;
}
