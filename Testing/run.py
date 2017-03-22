
import os
import sys
import json
from subprocess import call

import argparse
from pprint import pprint as pp
import math


def produce_game_environment():

    sys.stdout.write("Compiling game engine..\n")
    cmd = "cd ./environment; make; cd ../; cp ./environment/halite ./halite"
    call([cmd], shell=True)

    if not os.path.exists("halite"):
        sys.stderr.write("Failed to produce executable environment\n")
        sys.stderr.write("Corrupt archive?")
        exit(1)


def check_env():

    # if not os.path.exists("halite"):
    produce_game_environment()

    if os.path.exists("makefile") or os.path.exists("Makefile"):
        sys.stdout.write("Compiling player sources..\n")
        call(["make"], shell=True)

    call(["rm -rf *.hlt; rm -rf replays; mkdir -p replays;"], shell=True)


class HaliteEnv(object):

    def __init__(self,
                 bot_cmd,
                 height=30,
                 width=30,
                 seed=42,
                 max_turns=-1):

        self.bot_cmd   = bot_cmd
        self.height    = height
        self.width     = width
        self.seed      = seed
        self.max_turns = max_turns

    def __add_map(self, cmd):

        cmd += "-d \"" + str(self.height) + " " + str(self.width) + "\" "
        cmd += "-s " + str(self.seed)
        return cmd

    def __add_bot(self, cmd, bot_cmd):

        cmd += " \"" + bot_cmd + "\""
        return cmd

    def __add_turn_limit(self, cmd):

        if self.max_turns > 0:
            cmd += " --max_turns " + str(self.max_turns) + " "

        return cmd

    def run(self):

        cmd = "./halite -q "
        cmd = self.__add_map(cmd)
        cmd = self.__add_turn_limit(cmd)

        cmd = self.__add_bot(cmd, self.bot_cmd)
        call([cmd], shell=True)

        result = None
        for file in os.listdir("./"):
            if file.endswith(".hlt"):
                result = file

        if result is None:
            sys.stderr.write("There was an error during the game, no valid replay file was produced!\n")
            return None

        return result


def default_map_limit(height, width):
    return int(math.sqrt(height * width) * 10)


def compute_score(player_result, soft_limit, hard_limit, game_weight):
    return game_weight * (1 - (player_result - soft_limit) / (hard_limit - soft_limit))


def round_one(cmd):

    sys.stdout.write("Round 1 - single player map conquest!\n")

    env   = HaliteEnv(cmd)
    games = [
            (15, 20, 42, 175, 200),  # Try to beat 120
            (20, 15, 42, 175, 200),  # Try to beat 120
            (30, 30, 42, 250, 300),  # Try to beat 150
            (40, 40, 42, 275, 400),  # Try to beat 200
            (50, 50, 42, 300, 500),  # Try to beat 200
            # (50, 50, 123456789, 300.0, 500.0)  # Try to beat the world record of 154 frames
    ]

    max_score    = 0.5                     # Round score
    game_weight  = max_score / len(games)  # Equal weight / game
    player_score = 0.0

    for height, width, seed, soft_limit, hard_limit in games:

        env.height     = height
        env.width      = width
        env.seed       = seed
        env.max_turns  = hard_limit

        log = env.run()
        if log is None:
            continue

        with open(log, "r") as f:
            result = json.loads(f.read())

            if result["map_conquered"]:
                sys.stdout.write("Map conquered in " + str(result["num_frames"]) + "!\n")

                player_result = result["num_frames"]

                if result["num_frames"] <= soft_limit:
                    points = game_weight
                elif result["num_frames"] < hard_limit:
                    points = compute_score(float(player_result),
                                           float(soft_limit),
                                           float(hard_limit),
                                           game_weight)
                else:
                    points = 0.0

                player_score += points

            else:
                points = 0.0
                sys.stdout.write("\n" + result["player_names"][0] + " failed to conquer every productive tile on the map!\n")

            sys.stdout.write("Map score: " + str(points) + "\n")

        call(["mv " + log + " ./replays"], shell=True)

    sys.stdout.write("Round 1 - done!\n")
    sys.stdout.write("Final score: " + str(player_score) + "/" + str(max_score) + "\n")


def round_two():
    pass


def round_three():
    pass


def cleanup():
    call(["rm -f *.hlt; rm -rf replays/; rm -f *.log"], shell=True)
    if os.path.exists("makefile") or os.path.exists("Makefile"):
        call(["make clean"], shell=True)


def main():

    parser = argparse.ArgumentParser(description='PA project evaluator')
    parser.add_argument('--cmd', required=True, help="Command line instruction to execute the bot. eg: ./MyBot")
    parser.add_argument('--round', type=int, default=1, help="Round index (1, 2, or 3), default 1")
    parser.add_argument('--clean', action="store_true", help="Remove logs/game results, call make clean")
    args = parser.parse_args()

    check_env()

    rounds = [round_one, round_two, round_three]

    if args.round < 1 or args.round > len(rounds):
        sys.stderr.write("Invalid round parameter (should be an integer in [1, 3])\n")
        exit(1)

    # Let the games begin!
    rounds[args.round - 1](args.cmd)

    if args.clean:
        cleanup()

if __name__ == "__main__":
    main()
