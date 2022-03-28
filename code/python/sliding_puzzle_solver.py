import threading, copy, time

from pathlib import Path
from collections import deque
from enum import Enum, auto

import pyjson5


class Direction(Enum):
	UP = auto()
	DOWN = auto()
	LEFT = auto()
	RIGHT = auto()


class SlidingPuzzleSolver:
	def __init__(self, puzzle_name):
		puzzle_json = self.get_puzzle_json(puzzle_name)
		self.initialize_constant_fields(puzzle_json)
		self.initialize_variable_fields()


	def run(self):
		self.print_board()

		starting_positions = self.get_starting_positions()
		self.add_new_state(starting_positions)
		self.solve(starting_positions)

		print("\nDone!")


	def get_puzzle_json(self, puzzle_name):
		with open("puzzles" / Path(puzzle_name).with_suffix(".jsonc"), "r") as f:
			return pyjson5.decode_io(f)


	def initialize_constant_fields(self, puzzle_json):
		BOARD_SIZE = puzzle_json["board_size"]
		self.WIDTH = BOARD_SIZE["width"]
		self.HEIGHT = BOARD_SIZE["height"]

		self.PIECES = puzzle_json["pieces"]

		self.PIECE_ENDINGS = puzzle_json["piece_endings"]

		self.START_TIME = time.time()

		self.EMPTY_CHARACTER = " "

		self.DIRECTION_CHARACTERS = {
			Direction.UP: "^",
			Direction.DOWN: "v",
			Direction.LEFT: "<",
			Direction.RIGHT: ">",
		}


	def initialize_variable_fields(self):
		self.states = set()
		self.state_count = 0
		self.prev_state_count = 0

		self.running = True
		self.finished = False


	def get_starting_positions(self):
		starting_positions = {}

		for PIECE_LABEL, PIECE in self.PIECES.items():
			starting_positions[PIECE_LABEL] = {
				"pos": {
					"x": PIECE["pos"]["x"],
					"y": PIECE["pos"]["y"]
				}
			}

		return starting_positions


	def add_new_state(self, pieces):
		state = repr(pieces)

		if state not in self.states:
			self.states.add(state)
			return True

		return False


	def print_board(self):
		board = self.get_board()

		for row in board:
			print(row)

		print()


	def get_board(self):
		board = [[self.EMPTY_CHARACTER] * self.WIDTH for _ in range(self.HEIGHT)]

		for PIECE_LABEL, PIECE in self.PIECES.items():
			POS, SIZE = PIECE["pos"], PIECE["size"]

			Y = POS["y"]
			HEIGHT = SIZE["height"]

			for y2 in range(Y, Y + HEIGHT):
				X = POS["x"]
				WIDTH = SIZE["width"]

				for x2 in range(X, X + WIDTH):
					board[y2][x2] = PIECE_LABEL

		return board


	def solve(self, starting_positions):
		queue = deque([ [starting_positions.copy(), []] ])

		self.timed_print(queue)

		while len(queue) > 0:
			pieces_positions, path = queue.popleft()

			self.update_finished(pieces_positions)

			if self.finished:
				finished_message = f"\nA shortest path of {len(path)} moves was found! {self.state_count} unique states were seen. The remaining queue length is {len(queue)}."
				print(finished_message)

				path_string = "".join(path)
				print(f"Path: {path_string}")

				break

			for piece_label, piece in pieces_positions.items():
				# Saves the position of the piece, in case it needs to be moved back
				piece_pos = piece["pos"]
				x = piece_pos["x"]
				y = piece_pos["y"]

				for direction in Direction:
					if self.is_valid_move(direction, piece_label, piece_pos, pieces_positions):
						new_pieces_positions = self.deepcopy_pieces_positions(pieces_positions)

						new_path_part = piece_label + self.DIRECTION_CHARACTERS[direction]

						queue.append([ new_pieces_positions, path + [new_path_part] ])

						# Moves the piece back
						piece["x"] = x
						piece["y"] = y

		self.running = False


	def update_finished(self, pieces_positions):
		self.finished = True

		for PIECE_LABEL, PIECE_ENDING in self.PIECE_ENDINGS.items():
			if pieces_positions[PIECE_LABEL] != PIECE_ENDING:
				self.finished = False


	def deepcopy_pieces_positions(self, pieces):
		deepcopied_pieces_positions = {}

		for piece_label, piece in pieces.items():
			piece_pos = piece["pos"]
			deepcopied_pieces_positions[piece_label] = {
				"pos": {
					"x": piece_pos["x"],
					"y": piece_pos["y"]
				}
			}

		return deepcopied_pieces_positions


	def timed_print(self, queue):
		if self.running:
			threading.Timer(1, self.timed_print, [queue]).start()

		if not self.finished:
			elapsed_time = int(time.time() - self.START_TIME)

			states_count_diff = self.state_count - self.prev_state_count
			self.prev_state_count = self.state_count

			print(
				f"\rElapsed time: {elapsed_time} seconds"
				f", Unique states: {self.state_count} (+{states_count_diff}/s)"
				f", Queue length: {len(queue)}",
				end="",
				flush=True
			)


	def is_valid_move(self, direction, piece_label, piece_pos, pieces):
		# Saves the position of the piece, in case it needs to be moved back
		x = piece_pos["x"]
		y = piece_pos["y"]

		self.move(direction, piece_pos)

		if (
			self.move_doesnt_cross_puzzle_edge(piece_label, piece_pos) and
			self.no_intersection(piece_label, piece_pos, pieces) and
			self.add_new_state(pieces)
		):
			self.state_count += 1
			return True

		# Moves the piece back
		piece_pos["x"] = x
		piece_pos["y"] = y

		return False


	# TODO: Possibly use lookup table for x and y instead if it's faster in C++
	def move(self, direction, piece_pos):
		match direction:
			case Direction.UP:
				piece_pos["y"] += -1
			case Direction.DOWN:
				piece_pos["y"] += 1
			case Direction.LEFT:
				piece_pos["x"] += -1
			case Direction.RIGHT:
				piece_pos["x"] += 1


	def move_doesnt_cross_puzzle_edge(self, piece_label, piece_pos):
		x = piece_pos["x"]
		y = piece_pos["y"]

		PIECE = self.PIECES[piece_label]

		PIECE_SIZE = PIECE["size"]
		PIECE_WIDTH = PIECE_SIZE["width"]
		PIECE_HEIGHT = PIECE_SIZE["height"]

		if y >= 0 and y + (PIECE_HEIGHT - 1) < self.HEIGHT and x >= 0 and x + (PIECE_WIDTH - 1) < self.WIDTH:
			return True
		return False


	def no_intersection(self, piece_label_1, piece1_pos, pieces):
		x1 = piece1_pos["x"]
		y1 = piece1_pos["y"]

		SIZE1 = self.PIECES[piece_label_1]["size"]
		W1 = SIZE1["width"]
		H1 = SIZE1["height"]

		p1t = y1
		p1b = y1 + H1

		p1l = x1
		p1r = x1 + W1

		for piece_label_2, piece2 in pieces.items():
			if piece_label_1 == piece_label_2:
				continue

			size2 = self.PIECES[piece_label_2]["size"]

			piece2_pos = piece2["pos"]
			x2 = piece2_pos["x"]
			y2 = piece2_pos["y"]

			W2 = size2["width"]
			H2 = size2["height"]

			p2t = y2
			p2b = y2 + H2

			p2l = x2
			p2r = x2 + W2

			if p1r > p2l and p1l < p2r and p1b > p2t and p1t < p2b:
				return False

		return True
