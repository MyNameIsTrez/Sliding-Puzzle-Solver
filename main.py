from enum import Enum, auto

import logging

# import sys
# sys.setrecursionlimit(1500)
# logging.info(sys.getrecursionlimit())


CHOSEN_PUZZLE = "klotski"

EMPTY_CHARACTER = " "


PUZZLES = {
	"klotski": {
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


def print_board():
	board = get_board()

	print()
	for row in board:
		print(row)


def is_intersection():
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
					return True

				board[y2][x2] = piece_label

	return False


def get_state():
	state = ""

	for piece_label, piece in PIECES.items():
		state += piece_label

		pos = piece["pos"]
		# TODO: Are an X and Y state never going to mix, aka 69 = 6 9? (I don't think so?)
		state += str(pos["x"])
		state += str(pos["y"])

	return state


# TODO:
# Efficiently store which states have already been visited to avoid infinite loops
def is_new_state(state, seen_states):
	return state not in seen_states


# TODO: Replace with a lookup/jump table?
def can_move(dir, piece):
	pos = piece["pos"]
	x = pos["x"]
	y = pos["y"]

	size = piece["size"]
	width = size["width"]
	height = size["height"]

	if dir == Direction.UP and y >= 1:
		return True
	elif dir == Direction.DOWN and y + height < HEIGHT:
		return True
	elif dir == Direction.LEFT and x >= 1:
		return True
	elif dir == Direction.RIGHT and x + width < WIDTH:
		return True
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


def solve(seen_states):
	for piece_label, piece in PIECES.items():
		# save pos of piece
		pos = piece["pos"]
		x = pos["x"]
		y = pos["y"]

		# logging.info("")
		logging.info(piece_label)
		logging.info(piece)

		# for move piece up/down/left/right:
		for dir in Direction:
			if can_move(dir, piece): # A piece in the top-left corner of the puzzle can't move up nor left
				move(dir, pos)
			else:
				continue # TODO: Is continuing here correct?

			logging.info(dir)
			logging.info(piece)

			state = get_state()

			is_a_new_state = is_new_state(state, seen_states)

			if state not in seen_states:
				seen_states.append(state)

			logging.info(f"is_intersection: {is_intersection()}")
			logging.info(f"not is_a_new_state: {not is_a_new_state}")
			if is_intersection() or not is_a_new_state:
				pos["x"] = x
				pos["y"] = y
				logging.info("Taking a different direction")
			else:
				logging.info("MOVED\n")
				logging.info("RECURSING\n")
				print_board()
				solve(seen_states)

		# TODO: Move this return to its right spot, wherever that may be
		# return

	logging.info("BACKTRACKING\n")

	print("Backtracking")


def main():
	logging.basicConfig(filename='main.log', encoding='utf-8', level=logging.DEBUG, filemode="w")

	seen_states = [ get_state() ]

	print_board()

	solve(seen_states)

	print("\n\nSOLUTION FOUND!")

	# TODO: Why is this completely different than the previously printed board?
	print_board()


if __name__ == "__main__":
	main()