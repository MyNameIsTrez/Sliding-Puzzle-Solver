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
		self.add_new_state(self.STARTING_PIECES_POSITIONS)

		self.solve()

		print("\nDone!")


	def get_puzzle_json(self, puzzle_name):
		with open("puzzles" / Path(puzzle_name).with_suffix(".jsonc"), "r") as f:
			return pyjson5.decode_io(f)


	def initialize_constant_fields(self, puzzle_json):
		BOARD_SIZE = puzzle_json["board_size"]
		self.WIDTH = BOARD_SIZE["width"]
		self.HEIGHT = BOARD_SIZE["height"]

		self.STARTING_PIECES = puzzle_json["starting_pieces"]
		self.set_starting_pieces_positions()
		self.ENDING_PIECES = puzzle_json["ending_pieces"]

		self.START_TIME = time.time()

		self.EMPTY_CHARACTER = " "

		self.DIRECTION_CHARACTERS = {
			Direction.UP: "^",
			Direction.DOWN: "v",
			Direction.LEFT: "<",
			Direction.RIGHT: ">",
		}

		self.PRINT_BOARD = False


	def initialize_variable_fields(self):
		self.states = set()
		self.state_count = 0
		self.prev_state_count = 0

		self.running = True
		self.finished = False


	def set_starting_pieces_positions(self):
		self.STARTING_PIECES_POSITIONS = {}

		for STARTING_PIECE_LABEL, STARTING_PIECE in self.STARTING_PIECES.items():
			STARTING_PIECE_POS = STARTING_PIECE["pos"]

			self.STARTING_PIECES_POSITIONS[STARTING_PIECE_LABEL] = {
				"pos": {
					"x": STARTING_PIECE_POS["x"],
					"y": STARTING_PIECE_POS["y"]
				}
			}


	def add_new_state(self, pieces):
		state = repr(pieces)

		if state not in self.states:
			self.states.add(state)
			return True

		return False


	def print_board(self, pieces):
		board = self.get_board(pieces)

		for row in board:
			print(row)

		print()


	def get_board(self, pieces):
		board = [[self.EMPTY_CHARACTER] * self.WIDTH for _ in range(self.HEIGHT)]

		for piece_label, piece in pieces.items():
			pos = piece["pos"]
			size = self.STARTING_PIECES[piece_label]["size"]

			y = pos["y"]
			height = size["height"]

			for y2 in range(y, y + height):
				x = pos["x"]
				width = size["width"]

				for x2 in range(x, x + width):
					board[y2][x2] = piece_label

		return board


	def solve(self):
		queue = deque([ [self.STARTING_PIECES_POSITIONS, []] ])

		self.timed_print(queue)

		while len(queue) > 0:
			pieces, path = queue.popleft()

			if self.PRINT_BOARD:
				self.print_board(pieces)

			self.update_finished(pieces)

			if self.finished:
				finished_message = f"\nA shortest path of {len(path)} moves was found! {self.state_count} unique states were seen. The remaining queue length is {len(queue)}."
				print(finished_message)

				path_string = "".join(path)
				print(f"Path: {path_string}")

				break

			for piece_label, piece in pieces.items():
				# Saves the position of the piece, in case it needs to be moved back
				piece_pos = piece["pos"]
				x = piece_pos["x"]
				y = piece_pos["y"]

				for direction in Direction:
					if self.is_valid_move(direction, piece_label, piece_pos, pieces):
						new_pieces_positions = self.deepcopy_pieces_positions(pieces)

						new_path_part = piece_label + self.DIRECTION_CHARACTERS[direction]

						queue.append([ new_pieces_positions, path + [new_path_part] ])

						# Moves the piece back
						piece["x"] = x
						piece["y"] = y

		self.running = False


	def update_finished(self, pieces):
		self.finished = True

		for PIECE_LABEL, ENDING_PIECE in self.ENDING_PIECES.items():
			if pieces[PIECE_LABEL] != ENDING_PIECE:
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

		STARTING_PIECE = self.STARTING_PIECES[piece_label]

		STARTING_PIECE_SIZE = STARTING_PIECE["size"]
		STARTING_PIECE_WIDTH = STARTING_PIECE_SIZE["width"]
		STARTING_PIECE_HEIGHT = STARTING_PIECE_SIZE["height"]

		if y >= 0 and y + (STARTING_PIECE_HEIGHT - 1) < self.HEIGHT and x >= 0 and x + (STARTING_PIECE_WIDTH - 1) < self.WIDTH:
			return True
		return False


	def no_intersection(self, piece_label_1, piece1_pos, pieces):
		x1 = piece1_pos["x"]
		y1 = piece1_pos["y"]

		SIZE1 = self.STARTING_PIECES[piece_label_1]["size"]
		W1 = SIZE1["width"]
		H1 = SIZE1["height"]

		p1t = y1
		p1b = y1 + H1

		p1l = x1
		p1r = x1 + W1

		for piece_label_2, piece2 in pieces.items():
			if piece_label_1 == piece_label_2:
				continue

			piece2_pos = piece2["pos"]
			x2 = piece2_pos["x"]
			y2 = piece2_pos["y"]

			SIZE2 = self.STARTING_PIECES[piece_label_2]["size"]
			W2 = SIZE2["width"]
			H2 = SIZE2["height"]

			p2t = y2
			p2b = y2 + H2

			p2l = x2
			p2r = x2 + W2

			if p1r > p2l and p1l < p2r and p1b > p2t and p1t < p2b:
				return False

		return True
