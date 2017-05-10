#!/bin/bash
MyBot="202_1.exe"
IronBot="starkbot_linux_x64"
DBot="DBotv4_linux_x64"
SEEDS=(3 4 5 8 10)
RESULTS=(-1 -1 -1 -1 -1)
SCORES=(0 0 0 0 0)

echo "#########  Initializing black magic  #########"

#Round 1
./halite -d "40 40" -s ${SEEDS[0]} ./${MyBot} ./${IronBot} ./${DBot} ./${DBot} > Round1.out
RESULTS[0]=$(cat Round1.out | tail -4 | head -1|cut -d'#' -f4|cut -c-3)
echo "Round 1: ${RESULTS[0]} turns"
SCORES[0]=$(echo print $((RESULTS[0]- 100)) **2 |python)

#Round 2
./halite -d "40 40" -s ${SEEDS[1]} ./${MyBot} ./${IronBot} ./${IronBot} ./${IronBot} > Round2.out
RESULTS[1]=$(cat Round2.out | tail -4 | head -1|cut -d'#' -f4|cut -c-3)
echo "Round 2: ${RESULTS[1]} turns"
SCORES[1]=$(echo print $((RESULTS[1]- 100)) **2 |python)

#Round 3
./halite -d "40 40" -s ${SEEDS[2]} ./${MyBot} ./${DBot} ./${IronBot} ./${DBot} > Round3.out
RESULTS[2]=$(cat Round3.out | tail -4 | head -1|cut -d'#' -f4|cut -c-3)
echo "Round 3: ${RESULTS[2]} turns"
SCORES[2]=$(echo print $((RESULTS[2]- 100)) **2 |python)

#Round 4
./halite -d "40 40" -s ${SEEDS[3]} ./${MyBot} ./${DBot} ./${DBot} ./${DBot} > Round4.out
RESULTS[3]=$(cat Round4.out | tail -4 | head -1|cut -d'#' -f4|cut -c-3)
echo "Round 4: ${RESULTS[3]} turns"
SCORES[3]=$(echo print $((RESULTS[3]- 100)) **2 |python)

#Round 5
./halite -d "40 40" -s ${SEEDS[4]} ./${MyBot} ./${DBot} ./${DBot} ./${IronBot} > Round5.out
RESULTS[4]=$(cat Round5.out | tail -4 | head -1|cut -d'#' -f4|cut -c-3)
echo "Round 5: ${RESULTS[4]} turns"
SCORES[4]=$(echo print $((RESULTS[4]- 100)) **2  |python)

#Final Score
echo "### Final score: $(echo print $((SCORES[0] + SCORES[1] + SCORES[2] + SCORES[3] + SCORES[4])) *0.4/50000|python)/0.4 ###"

