from enum import Enum, auto

import logging


# import sys
# sys.setrecursionlimit(10**7) # max depth of recursion


# Max recursion depth
# Setting this too high will cause the program to silently crash (probably called a segfault)
# 1572 is safe on my Windows 10 PC according to find_recursionlimit.py
# sys.setrecursionlimit(1900)

# import threading
# threading.stack_size(2**27)  # new thread will get stack of such size


CHOSEN_PUZZLE = "klotski"

EMPTY_CHARACTER = " "


PUZZLES = {
	"klotski": {
#		Starting state: A00B10C30D02E12F32G13H23I04J34

#		Move G: A00B10C30D02E12F32G14H23I04J34
#		Move H: A00B10C30D02E12F32G13H24I04J34
#		Move I: A00B10C30D02E12F32G13H23I14J34
#		Move J: A00B10C30D02E12F32G13H23I04J24

		"BOARD_SIZE": {
			"WIDTH": 4,
			"HEIGHT": 5
		},

#		ABBC
#		ABBC
#		DEEF
#		DGHF
#		I  J
		"PIECES": {
			"A": { "pos": { "x": 0, "y": 0 }, "size": { "width": 1, "height": 2 } },
			"B": { "pos": { "x": 1, "y": 0 }, "size": { "width": 2, "height": 2 } },
			"C": { "pos": { "x": 3, "y": 0 }, "size": { "width": 1, "height": 2 } },
			"D": { "pos": { "x": 0, "y": 2 }, "size": { "width": 1, "height": 2 } },
			"E": { "pos": { "x": 1, "y": 2 }, "size": { "width": 2, "height": 1 } },
			"F": { "pos": { "x": 3, "y": 2 }, "size": { "width": 1, "height": 2 } },
			"G": { "pos": { "x": 1, "y": 3 }, "size": { "width": 1, "height": 1 } },
			"H": { "pos": { "x": 2, "y": 3 }, "size": { "width": 1, "height": 1 } },
			"I": { "pos": { "x": 0, "y": 4 }, "size": { "width": 1, "height": 1 } },
			"J": { "pos": { "x": 3, "y": 4 }, "size": { "width": 1, "height": 1 } },
		}
	}
}


class Direction(Enum):
	UP = auto()
	DOWN = auto()
	LEFT = auto()
	RIGHT = auto()


PUZZLE = PUZZLES[CHOSEN_PUZZLE]

BOARD_SIZE = PUZZLE["BOARD_SIZE"]
WIDTH = BOARD_SIZE["WIDTH"]
HEIGHT = BOARD_SIZE["HEIGHT"]

PIECES = PUZZLE["PIECES"]


def main():
	logging.basicConfig(filename="main.log", encoding="utf-8", level=logging.DEBUG, filemode="w")

	state = get_state()
	states = [ state ]

	logging.info(f"State number: 1, State: {state}")
	print(f"\nState number: 1, State: {state}")

	print_board()

	solve(states)

	logging.info("Done")

	# print("\n\nSOLUTION FOUND!")

	# TODO: Why is this identical to the previously printed board?
	print_board()

	print(f"\nNumber of states: {len(states)}")


def print_board():
	board = get_board()

	for row in board:
		print(row)

	print()


def get_board():
	board = [[EMPTY_CHARACTER] * WIDTH for _ in range(HEIGHT)]

	for piece_label, piece in PIECES.items():
		pos, size = piece["pos"], piece["size"]

		y = pos["y"]
		height = size["height"]

		for y2 in range(y, y + height):
			x = pos["x"]
			width = size["width"]

			for x2 in range(x, x + width):
				board[y2][x2] = piece_label

	return board


def solve(states, depth=0):
	# piece_moved_at_least_once = False

	for piece_label, piece in PIECES.items():
		# save pos of piece
		pos = piece["pos"]
		x = pos["x"]
		y = pos["y"]

		logging.info(piece_label)
		logging.info(piece)

		if (
			piece_label == "J" and x == 3 and y == 4 and

			PIECES["A"]["pos"]["x"] == 0 and PIECES["A"]["pos"]["y"] == 0 and
			PIECES["B"]["pos"]["x"] == 1 and PIECES["B"]["pos"]["y"] == 0 and
			PIECES["C"]["pos"]["x"] == 3 and PIECES["C"]["pos"]["y"] == 0 and
			PIECES["D"]["pos"]["x"] == 0 and PIECES["D"]["pos"]["y"] == 2 and
			PIECES["E"]["pos"]["x"] == 1 and PIECES["E"]["pos"]["y"] == 2 and
			PIECES["F"]["pos"]["x"] == 3 and PIECES["F"]["pos"]["y"] == 2 and
			PIECES["G"]["pos"]["x"] == 1 and PIECES["G"]["pos"]["y"] == 3 and
			PIECES["H"]["pos"]["x"] == 2 and PIECES["H"]["pos"]["y"] == 3 and
			PIECES["I"]["pos"]["x"] == 0 and PIECES["I"]["pos"]["y"] == 4
			):
			print("Evaluating J at the bottom-right corner")
			print("foo")

		# for move piece up/down/left/right:
		for dir in Direction:
			logging.info(dir)

			if is_valid_move(dir, piece, states):
				logging.info(f"Depth: {depth}")
				print(f"Depth: {depth}")

				logging.info(piece)

				# print("a1")
				logging.info("MOVED, RECURSING")

				print_board()

				solve(states, depth + 1)

				pos["x"] = x
				pos["y"] = y

				print_board()

				# print("a2")

			# print("a3")
		# print("a4")

	# print("a5")

	logging.info("Backtracking due to not having moved and having no more pieces that can be moved")

	print("Backtracking due to not having moved and having no more pieces that can be moved")


def is_valid_move(dir, piece, states):
	# Saves the position of the piece in case it needs to be moved back
	pos = piece["pos"]
	x = pos["x"]
	y = pos["y"]

	move(dir, pos)

	state = get_state()

	if move_doesnt_cross_puzzle_edge(piece) and no_intersection() and is_new_state(state, states):
		states.append(state)

		logging.info(f"State number: {len(states)}, State: {state}")
		print(f"State number: {len(states)}, State: {state}")

		return True

	# Moves the piece back
	pos["x"] = x
	pos["y"] = y

	return False


# TODO: Replace with a lookup/jump table?
def move(dir, pos):
	if dir == Direction.UP:
		pos["y"] += -1
	elif dir == Direction.DOWN:
		pos["y"] += 1
	elif dir == Direction.LEFT:
		pos["x"] += -1
	elif dir == Direction.RIGHT:
		pos["x"] += 1


def get_state():
	state = ""

	for piece_label, piece in PIECES.items():
		state += piece_label

		pos = piece["pos"]
		# TODO: Are an X and Y state never going to mix, aka 69 = 6 9? (I don't think so?)
		state += str(pos["x"])
		state += str(pos["y"])

	return state


# TODO: Replace with a lookup/jump table?
def move_doesnt_cross_puzzle_edge(piece):
	pos = piece["pos"]
	x = pos["x"]
	y = pos["y"]

	size = piece["size"]
	width = size["width"]
	height = size["height"]

	if y >= 0 and y + (height - 1) < HEIGHT and x >= 0 and x + (width - 1) < WIDTH:
		return True
	return False


def no_intersection():
	board = [[EMPTY_CHARACTER] * WIDTH for _ in range(HEIGHT)]

	for piece_label, piece in PIECES.items():
		pos, size = piece["pos"], piece["size"]

		y = pos["y"]
		height = size["height"]

		for y2 in range(y, y + height):
			x = pos["x"]
			width = size["width"]

			for x2 in range(x, x + width):
				if board[y2][x2] != EMPTY_CHARACTER:
					return False

				board[y2][x2] = piece_label

	return True


# TODO:
# Efficiently store which states have already been visited to avoid infinite loops
def is_new_state(state, states):
	return state not in states


if __name__ == "__main__":
    main()

	# import sys
	# sys.setrecursionlimit(10**7) # max depth of recursion

	# import threading
	# from threading import Thread

	# threading.stack_size(2**27)
	# thread = threading.Thread(target = main)
	# print(threading.stack_size())
	# thread.start()
	# thread.join()
	# print("Thread finished")