import logging, threading, ujson

from enum import Enum, auto


def main():
	# filename has to be set or everything will be printed to the terminal
	# level has to be set for logging.info()
	# filemode default to "a"
	logging.basicConfig(filename="main.log", level=logging.INFO, filemode="w")

	logging.disable() # The code is significantly faster without logging

	state = get_state(PIECES)
	STATES.append(state)

	logging.info(f"State number: 1, State: {state}")
	print(f"\nState number: 1, State: {state}")

	print_board(PIECES)

	solve()

	logging.info("Done")

	# print("\n\nSOLUTION FOUND!")

	# TODO: Why is this identical to the previously printed board?
	# print_board(pieces)

	print(f"\nNumber of states: {len(STATES)}")


def get_state(pieces):
	state = ""

	for piece_label, piece in pieces.items():
		state += piece_label

		pos = piece["pos"]
		# TODO: Are an X and Y state never going to mix, aka 69 = 6 9? (I don't think so?)
		state += str(pos["x"])
		state += str(pos["y"])

	return state


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


# Traverses using depth-first search on the last element on the stack
def solve():
	stack = [ { "pieces": ujson.loads(ujson.dumps(PIECES)), "path": [] } ]

	# timed_print_stack_path(stack)

	while True:
		# TODO: The debugger shows weird "special variables" being created.
		# It should be faster to create a custom copier.
		node = ujson.loads(ujson.dumps(stack.pop()))

		solve_2(node, stack)

		# logging.info("Backtracking due to not having moved and having no more pieces that can be moved")

		if len(stack) == 0:
			break

		# For performance profiling with Austin
		if len(stack) > 1000:
			break

		#print("Backtracking due to not having moved and having no more pieces that can be moved")


def timed_print_stack_path(stack):
	threading.Timer(1, timed_print_stack_path, [stack]).start()

	path_string = "".join(stack[-1]["path"][:50])

	print(f"progress stack length: {len(stack)}, {path_string}", end="\r", flush=True)
	# print(f"{stack[-40:]}")


def solve_2(node, stack):
	depth = len(stack)

	pieces = node["pieces"]

	for piece_label, piece in pieces.items():
		logging.info(piece_label)
		logging.info(piece)

		# Save the position of the piece for moving back
		pos = piece["pos"]
		x = pos["x"]
		y = pos["y"]

		# For moving a piece up/down/left/right
		for direction in Direction:
			logging.info(direction)

			if is_valid_move(direction, piece, pieces):
				logging.info(f"Depth: {depth}")
				# print(f"Depth: {depth}")

				logging.info(piece)

				logging.info("MOVED, RECURSING")

				#print_board(pieces)

				new_node = ujson.loads(ujson.dumps(node))

				piece_path = f"{piece_label}{DIRECTION_CHARACTERS[direction]} "

				if len(new_node["path"]) < 100: # This check speeds up the program significantly.
					new_node["path"].append(piece_path)

				stack.append(new_node)

				pos["x"] = x
				pos["y"] = y

				#print_board(pieces)


def is_valid_move(direction, piece, pieces):
	# Saves the position of the piece in case it needs to be moved back
	pos = piece["pos"]
	x = pos["x"]
	y = pos["y"]

	move(direction, pos)

	state = get_state(pieces)

	if move_doesnt_cross_puzzle_edge(piece) and no_intersection(pieces) and is_new_state(state):
		STATES.append(state)

		logging.info(f"State number: {len(STATES)}, State: {state}")
		# print(f"State number: {len(STATES)}, State: {state}")

		return True

	# Moves the piece back
	pos["x"] = x
	pos["y"] = y

	return False


# TODO: Replace with a lookup/jump table?
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


def no_intersection(pieces):
	for piece_label_1, piece1 in pieces.items():
		pos1 = piece1["pos"]
		size1 = piece1["size"]

		x1 = pos1["x"]
		y1 = pos1["y"]

		w1 = size1["width"]
		h1 = size1["height"]

		p1t = y1
		p1b = y1 + h1

		p1l = x1
		p1r = x1 + w1

		for piece_label_2, piece2 in pieces.items():
			if piece_label_1 == piece_label_2:
				continue

			pos2 = piece2["pos"]
			size2 = piece2["size"]

			x2 = pos2["x"]
			y2 = pos2["y"]

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


def is_new_state(state):
	return state not in STATES


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

	STATES = []

	main()