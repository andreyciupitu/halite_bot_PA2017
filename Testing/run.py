
import os
import sys
import json
from subprocess import call

import argparse
import math
from pprint import pprint as pp

import platform


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


def view_replay(browser, log):

    output_filename = log.replace(".hlt", ".htm")
    path_to_file    = os.path.join("visualizer", output_filename)

    if not os.path.exists(output_filename):

        # parse replay file
        with open(log, 'r') as f:
            replay_data = f.read()

        # parse template html
        with open(os.path.join("visualizer", "Visualizer.htm")) as f:
            html = f.read()

        html = html.replace("FILENAME", log)
        html = html.replace("REPLAY_DATA", replay_data)

        # dump replay html file
        with open(os.path.join("visualizer", output_filename), 'w') as f:
            f.write(html)

    with open("/dev/null") as null:
        call(browser + " " + path_to_file + " &", shell=True, stderr=null, stdout=null)


class HaliteEnv(object):

    def __init__(self,
                 player_bot_cmd,
                 height=30,
                 width=30,
                 seed=42,
                 max_turns=-1):

        self.bots      = [player_bot_cmd]
        self.height    = height
        self.width     = width
        self.seed      = seed
        self.max_turns = max_turns

    def __add_map(self, cmd):

        cmd += "-d \"{0} {1}\" ".format(self.width, self.height)
        cmd += "-s {0}".format(self.seed)
        return cmd

    def __add_bot(self, cmd, bot_cmd):

        cmd += " \"{0}\"".format(bot_cmd)
        return cmd

    def __add_turn_limit(self, cmd):

        if self.max_turns > 0:
            cmd += " --max_turns {0} ".format(self.max_turns)
        return cmd

    def run(self):

        sys.stdout.write("Map: Height {0}, Width {1}, Seed {2}\n".format(self.height, self.width, self.seed))

        cmd = "./halite -q "
        cmd = self.__add_map(cmd)
        cmd = self.__add_turn_limit(cmd)

        for bot in self.bots:
            cmd = self.__add_bot(cmd, bot)

        call([cmd], shell=True)

        result = None
        for file in os.listdir("./"):
            if file.endswith(".hlt"):
                result = file

        if result is None:
            sys.stderr.write("There was an error during the game, "
                             "no valid replay file was produced!\n")
            return None

        return result


def default_map_limit(height, width):
    return int(math.sqrt(height * width) * 10)


def compute_score(player_result, soft_limit, hard_limit, game_weight):
    return game_weight * (1 - (player_result - soft_limit) / (hard_limit - soft_limit))


def round_one(cmd, browser=None):

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

        if browser:
            view_replay(browser, log)

        call(["mv " + log + " ./replays"], shell=True)

    sys.stdout.write("Round 1 - done!\n")
    sys.stdout.write("Final score: " + str(player_score) + "/" + str(max_score) + "\n")


def round_two(cmd, browser=None):

    sys.stdout.write("Round 2 - 1vs1 battles!\n")
    # https://ocw.cs.pub.ro/courses/pa/regulament-proiect

    env = HaliteEnv(cmd)
    games = [
        (28, 24, 314, 0.1),
        (30, 30, 42, 0.1),
        (40, 40, 154, 0.1),
        (30, 50, 3, 0.1),
        (50, 50, 42, 0.1),
    ]

    arch, _ = platform.architecture()
    env.bots.append("./bots/DBot_linux_" + arch)

    player_score = 0.0

    for height, width, seed, points in games:

        env.height = height
        env.width  = width
        env.seed   = seed

        log = env.run()

        with open(log, "r") as f:

            result = json.loads(f.read())
            if result["winner"] != "DBotv2":
                sys.stdout.write("Victory")
                player_score += points
            else:
                sys.stdout.write("Defeat")

            sys.stdout.write(" in {0} steps!\n\n".format(result["num_frames"] - 1))

        if browser:
            view_replay(browser, log)

        call(["mv {0} ./replays".format(log)], shell=True)

    sys.stdout.write("Round 2 - done!\n")
    sys.stdout.write("Final score: {0}\n".format(player_score))


def round_three(cmd, browser=None):
    pass


def cleanup():
    call(["rm -f *.hlt; rm -rf replays/; rm -f *.log"], shell=True)
    if os.path.exists("makefile") or os.path.exists("Makefile"):
        call(["make clean"], shell=True)


def main():

    parser = argparse.ArgumentParser(description='PA project evaluator')
    parser.add_argument('--cmd', required=True, help="Command line instruction to execute the bot. eg: ./MyBot")
    parser.add_argument('--round', type=int, default=2, help="Round index (1, 2, or 3), default 1")
    parser.add_argument('--clean', action="store_true", help="Remove logs/game results, call make clean")
    parser.add_argument('--visualizer', help="Display replays in browser; eg. \"firefox\", \"google-chrome-stable\"")
    args = parser.parse_args()

    check_env()

    rounds = [round_one, round_two, round_three]

    if args.round < 1 or args.round > len(rounds):
        sys.stderr.write("Invalid round parameter (should be an integer in [1, 3])\n")
        exit(1)

    # Let the games begin!
    rounds[args.round - 1](args.cmd, args.visualizer)

    if args.clean:
        cleanup()

if __name__ == "__main__":
    main()