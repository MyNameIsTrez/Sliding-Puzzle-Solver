import logging, threading, copy, time

from enum import Enum, auto
from collections import deque


class Direction(Enum):
	UP = auto()
	DOWN = auto()
	LEFT = auto()
	RIGHT = auto()


def initialize_puzzles():
	global PUZZLES

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


def initialize_global_constants():
	global PUZZLE, EMPTY_CHARACTER, BOARD_SIZE, WIDTH, HEIGHT, PIECES, START_TIME, DIRECTION_CHARACTERS

	PUZZLE = PUZZLES[CHOSEN_PUZZLE]

	EMPTY_CHARACTER = " "

	BOARD_SIZE = PUZZLE["BOARD_SIZE"]
	WIDTH = BOARD_SIZE["WIDTH"]
	HEIGHT = BOARD_SIZE["HEIGHT"]

	PIECES = PUZZLE["PIECES"]

	START_TIME = time.time()

	DIRECTION_CHARACTERS = {
		Direction.UP: "^",
		Direction.DOWN: "v",
		Direction.LEFT: "<",
		Direction.RIGHT: ">",
	}


def initialize_global_non_constants():
	global states, state_count, prev_state_count, running, finished

	states = {}
	state_count = 0
	prev_state_count = 0

	running = True
	finished = False


def main():
	global state_count

	# filename has to be set or everything will be printed to the terminal
	# level has to be set for logging.info() to be written anywhere
	# filemode defaults to "a", so we change it to "w"
	logging.basicConfig(filename="main.log", level=logging.INFO, filemode="w")

	starting_positions = get_starting_positions()

	add_new_state(starting_positions)

	print_board(PIECES)

	solve(starting_positions)

	print("\nDone!")


def get_starting_positions():
	starting_positions = {}

	for piece_label, piece in PIECES.items():
		starting_positions[piece_label] = {
			"x": piece["pos"]["x"],
			"y": piece["pos"]["y"]
		}

	return starting_positions


def add_new_state(pieces):
	state = states

	new_state = False

	for piece in pieces.values():
		x = piece["x"]
		y = piece["y"]

		piece_coordinate_index = x + y * WIDTH

		if piece_coordinate_index not in state:
			new_state = True
			state[piece_coordinate_index] = {}
		state = state[piece_coordinate_index]

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


def solve(starting_positions):
	global running, finished

	queue = deque([ [starting_positions.copy(), []] ])

	timed_print(queue)

	while len(queue) > 0:
		pieces_positions, path = queue.popleft()

		b_piece_position = pieces_positions["B"]
		finished = b_piece_position["x"] == 1 and b_piece_position["y"] == 3
		if finished:
			path_string = "".join(path)
			print(f"\nA shortest path of {len(path)} moves was found! The remaining queue length is {len(queue)}. {len(states)} unique states were seen.")
			print(f"Path: {path_string}")
			logging.info(f"A shortest path of {len(path)} moves was found! The remaining queue length is {len(queue)}. {len(states)} unique states were seen.")
			logging.info(f"Path: {path_string}")
			break # Stops the program after one of the multiple shortest paths has been found

		# Uncomment this when you want to profile the code
		# if state_count > 20000:
		# 	break

		for piece_label, piece in pieces_positions.items():
			# Saves the position of the piece, in case it needs to be moved back
			x = piece["x"]
			y = piece["y"]

			for direction in Direction:
				if is_valid_move(direction, piece_label, piece, pieces_positions):
					new_pieces_positions = deepcopy_pieces_positions(pieces_positions)

					new_path_part = f"{piece_label}{DIRECTION_CHARACTERS[direction]} "

					queue.append([ new_pieces_positions, path + [new_path_part] ])

					# Moves the piece back
					piece["x"] = x
					piece["y"] = y

	running = False


def deepcopy_pieces_positions(pieces):
	deepcopied_pieces_positions = {}

	for piece_label, piece in pieces.items():
		deepcopied_pieces_positions[piece_label] = {}
		deepcopied_piece_pos = deepcopied_pieces_positions[piece_label]

		deepcopied_piece_pos["x"] = piece["x"]
		deepcopied_piece_pos["y"] = piece["y"]

	return deepcopied_pieces_positions


def timed_print(queue):
	global prev_state_count

	if running:
		threading.Timer(1, timed_print, [queue]).start()

	if not finished:
		elapsed_time = int(time.time() - START_TIME)

		states_count_diff = state_count - prev_state_count
		prev_state_count = state_count

		print(
			f"\rElapsed time: {elapsed_time} seconds"
			f", Unique states: {state_count} (+{states_count_diff})"
			f", Queue length: {len(queue)}",
			end="",
			flush=True
		)


def is_valid_move(direction, piece_label, piece, pieces):
	global state_count

	# Saves the position of the piece, in case it needs to be moved back
	x = piece["x"]
	y = piece["y"]

	move(direction, piece)

	if move_doesnt_cross_puzzle_edge(piece_label, piece) and no_intersection(piece_label, piece, pieces) and add_new_state(pieces):
		state_count += 1
		return True

	# Moves the piece back
	piece["x"] = x
	piece["y"] = y

	return False


# TODO: Possibly use lookup table for x and y instead if it's faster in C++
def move(direction, piece):
	match direction:
		case Direction.UP:
			piece["y"] += -1
		case Direction.DOWN:
			piece["y"] += 1
		case Direction.LEFT:
			piece["x"] += -1
		case Direction.RIGHT:
			piece["x"] += 1


def move_doesnt_cross_puzzle_edge(piece_label, piece):
	x = piece["x"]
	y = piece["y"]

	PIECE = PIECES[piece_label]
	size = PIECE["size"]
	width = size["width"]
	height = size["height"]

	if y >= 0 and y + (height - 1) < HEIGHT and x >= 0 and x + (width - 1) < WIDTH:
		return True
	return False


def no_intersection(piece_label_1, piece1, pieces):
	size1 = PIECES[piece_label_1]["size"]

	x1 = piece1["x"]
	y1 = piece1["y"]

	w1 = size1["width"]
	h1 = size1["height"]

	p1t = y1
	p1b = y1 + h1

	p1l = x1
	p1r = x1 + w1

	for piece_label_2, piece2 in pieces.items():
		if piece_label_1 == piece_label_2:
			continue

		size2 = PIECES[piece_label_2]["size"]

		x2 = piece2["x"]
		y2 = piece2["y"]

		w2 = size2["width"]
		h2 = size2["height"]

		p2t = y2
		p2b = y2 + h2

		p2l = x2
		p2r = x2 + w2

		# rectOneRight > rectTwoLeft && rectOneLeft < rectTwoRight && rectOneBottom > rectTwoTop && rectOneTop < rectTwoBottom
		if p1r > p2l and p1l < p2r and p1b > p2t and p1t < p2b:
			return False

	return True


if __name__ == "__main__":
	CHOSEN_PUZZLE = "klotski"

	initialize_puzzles()

	initialize_global_constants()
	initialize_global_non_constants()

	main()