import logging, threading, copy, time

from enum import Enum, auto
from collections import deque


def main():
	global STATE_COUNT

	# filename has to be set or everything will be printed to the terminal
	# level has to be set for logging.info()
	# filemode defaults to "a"
	# logging.basicConfig(filename="main.log", level=logging.INFO, filemode="w")

	# logging.disable() # The code is significantly faster when logging is disabled

	add_new_state(PIECES)

	print_board(PIECES)

	solve()

	print("Done!")


def add_new_state(pieces):
	state = STATES

	new_state = False

	for piece in pieces.values():
		pos = piece["pos"]
		x = pos["x"]
		y = pos["y"]

		if x not in state:
			new_state = True
			state[x] = {}
		state = state[x]

		if y not in state:
			new_state = True
			state[y] = {}
		state = state[y]

	return new_state


def print_board(pieces):
	board = get_board(pieces)

	for row in board:
		print(row)

	print()


def get_board(pieces):
	board = [[EMPTY_CHARACTER] * WIDTH for _ in range(HEIGHT)]

	for piece_label, piece in pieces.items():
		pos, size = piece["pos"], piece["size"]

		y = pos["y"]
		height = size["height"]

		for y2 in range(y, y + height):
			x = pos["x"]
			width = size["width"]

			for x2 in range(x, x + width):
				board[y2][x2] = piece_label

	return board


def solve():
	global running

	queue = deque([ deepcopy_pieces(PIECES) ])

	timed_print(queue)

	while True:
		pieces = queue.popleft()

		for piece_label, piece in pieces.items():
			# Save the position of the piece for moving back
			pos = piece["pos"]
			x = pos["x"]
			y = pos["y"]

			for direction in Direction:
				if is_valid_move(direction, piece, pieces):
					new_pieces = deepcopy_pieces(pieces)

					queue.append(new_pieces)

					pos["x"] = x
					pos["y"] = y

		pieces_b_pos = pieces["B"]["pos"]
		puzzle_finished = pieces_b_pos["x"] == 1 and pieces_b_pos["y"] == 3

		if len(queue) == 0 or puzzle_finished: # TODO: Replace the while condition with this?
			print("Shortest path found!")
			break

	running = False


def deepcopy_pieces(pieces):
	deepcopied_pieces = {}

	for piece_label, piece in pieces.items():
		deepcopied_pieces[piece_label] = {}
		deepcopied_piece = deepcopied_pieces[piece_label]

		deepcopied_piece["pos"] = {}
		deepcopied_pos = deepcopied_piece["pos"]

		piece_pos = piece["pos"]
		deepcopied_pos["x"] = piece_pos["x"]
		deepcopied_pos["y"] = piece_pos["y"]

		deepcopied_piece["size"] = {}
		deepcopied_size = deepcopied_piece["size"]

		piece_size = piece["size"]
		deepcopied_size["width"] = piece_size["width"]
		deepcopied_size["height"] = piece_size["height"]

	return deepcopied_pieces


def timed_print(queue):
	if running:
		threading.Timer(1, timed_print, [queue]).start()

	elapsed_time = int(time.time() - START_TIME)

	states_count_diff = STATE_COUNT - timed_print.prev_states_count
	timed_print.prev_states_count = STATE_COUNT

	print(f"\rElapsed time: {elapsed_time} seconds, Unique states: {STATE_COUNT} (+{states_count_diff}), Queue length: {len(queue)}", end="", flush=True)
timed_print.prev_states_count = 0


def is_valid_move(direction, piece, pieces):
	global STATE_COUNT

	# Saves the position of the piece in case it needs to be moved back
	pos = piece["pos"]
	x = pos["x"]
	y = pos["y"]

	move(direction, pos)

	if move_doesnt_cross_puzzle_edge(piece) and no_intersection_python(pieces) and add_new_state(pieces):
		STATE_COUNT += 1

		return True

	# Moves the piece back
	pos["x"] = x
	pos["y"] = y

	return False


def move(direction, pos):
	match direction:
		case Direction.UP:
			pos["y"] += -1
		case Direction.DOWN:
			pos["y"] += 1
		case Direction.LEFT:
			pos["x"] += -1
		case Direction.RIGHT:
			pos["x"] += 1


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


def no_intersection_python(pieces):
	board = [[EMPTY_CHARACTER] * WIDTH for _ in range(HEIGHT)]

	for piece_label, piece in pieces.items():
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


# def no_intersection_c(pieces):
# 	for piece_label_1, piece1 in pieces.items():
# 		pos1 = piece1["pos"]
# 		size1 = piece1["size"]

# 		x1 = pos1["x"]
# 		y1 = pos1["y"]

# 		w1 = size1["width"]
# 		h1 = size1["height"]

# 		p1t = y1
# 		p1b = y1 + h1

# 		p1l = x1
# 		p1r = x1 + w1

# 		for piece_label_2, piece2 in pieces.items():
# 			if piece_label_1 == piece_label_2:
# 				continue

# 			pos2 = piece2["pos"]
# 			size2 = piece2["size"]

# 			x2 = pos2["x"]
# 			y2 = pos2["y"]

# 			w2 = size2["width"]
# 			h2 = size2["height"]

# 			p2t = y2
# 			p2b = y2 + h2

# 			p2l = x2
# 			p2r = x2 + w2

# 			# rectOneRight > rectTwoLeft && rectOneLeft < rectTwoRight && rectOneBottom > rectTwoTop && rectOneTop < rectTwoBottom
# 			if p1r > p2l and p1l < p2r and p1b > p2t and p1t < p2b:
# 				return False

# 	return True


if __name__ == "__main__":
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
				"J": { "pos": { "x": 3, "y": 4 }, "size": { "width": 1, "height": 1 } }
			}
		}
	}

	class Direction(Enum):
		UP = auto()
		DOWN = auto()
		LEFT = auto()
		RIGHT = auto()

	DIRECTION_CHARACTERS = {
		Direction.UP: "^",
		Direction.DOWN: "v",
		Direction.LEFT: "<",
		Direction.RIGHT: ">",
	}


	PUZZLE = PUZZLES[CHOSEN_PUZZLE]

	BOARD_SIZE = PUZZLE["BOARD_SIZE"]
	WIDTH = BOARD_SIZE["WIDTH"]
	HEIGHT = BOARD_SIZE["HEIGHT"]

	PIECES = PUZZLE["PIECES"]

	STATES = {}
	STATE_COUNT = 0

	START_TIME = time.time()

	running = True

	main()