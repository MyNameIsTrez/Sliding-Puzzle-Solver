import threading, copy, time

from pathlib import Path
from collections import deque

import pyjson5


class SlidingPuzzleSolver:
	def __init__(self, puzzle_name):
		puzzle_json = self.get_puzzle_json(puzzle_name)
		self.initialize_constant_fields(puzzle_json)
		self.initialize_variable_fields()


	def run(self):
		self.print_board(self.STARTING_PIECES)

		self.add_new_state(self.STARTING_PIECES)

		# self.solve()
		self.solve_and_print_path()

		print("\nDone!")


	def get_puzzle_json(self, puzzle_name):
		with open("puzzles" / Path(puzzle_name).with_suffix(".jsonc"), "r") as f:
			return pyjson5.decode_io(f)


	def initialize_constant_fields(self, puzzle_json):
		BOARD_SIZE = puzzle_json["board_size"]
		self.WIDTH = BOARD_SIZE["width"]
		self.HEIGHT = BOARD_SIZE["height"]

		self.STARTING_PIECES_INFO = puzzle_json["starting_pieces_info"]
		self.set_starting_pieces()
		self.ENDING_PIECES = puzzle_json["ending_pieces"]

		self.START_TIME = time.time()

		self.EMPTY_CHARACTER = " "

		self.DIRECTION_CHARACTERS = [ "^", "v", "<", ">" ]

		self.PRINT_BOARD_EVERY_PATH = False


	def initialize_variable_fields(self):
		self.states = set()
		self.state_count = 0
		self.prev_state_count = 0

		self.running = True
		self.finished = False


	def set_starting_pieces(self):
		self.STARTING_PIECES = {}

		for STARTING_PIECE_LABEL, STARTING_PIECE in self.STARTING_PIECES_INFO.items():
			STARTING_PIECE_POS = STARTING_PIECE["pos"]

			self.STARTING_PIECES[STARTING_PIECE_LABEL] = {
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
			size = self.STARTING_PIECES_INFO[piece_label]["size"]

			y = pos["y"]
			height = size["height"]

			for y2 in range(y, y + height):
				x = pos["x"]
				width = size["width"]

				for x2 in range(x, x + width):
					board[y2][x2] = piece_label

		return board


	def solve(self):
		pieces_queue = deque( [ self.STARTING_PIECES ] )

		self.timed_print(pieces_queue)

		while len(pieces_queue) > 0:
			pieces = pieces_queue.popleft()

			if self.PRINT_BOARD_EVERY_PATH:
				self.print_board(pieces)

			self.update_finished(pieces)

			if self.finished:
				print(
					f"\nA shortest path was found!\n"
					f"{self.state_count} unique states were seen.\n"
					"The remaining queue length is {len(pieces_queue)}."
				)
				break

			for piece_label, piece in pieces.items():
				piece_pos = piece["pos"]

				# Saves the position of the piece for when it needs to be moved back.
				x = piece_pos["x"]
				y = piece_pos["y"]

				for direction in range(4):
					self.move(direction, piece_pos)

					if self.is_valid_move(piece_label, piece_pos, pieces):
						new_pieces_positions = self.deepcopy_pieces_positions(pieces)

						pieces_queue.append(new_pieces_positions)

					# Moves the piece back.
					piece_pos["x"] = x
					piece_pos["y"] = y

		self.running = False


	def solve_and_print_path(self):
		pieces_queue = deque([ [self.STARTING_PIECES, []] ])

		self.timed_print(pieces_queue)

		while len(pieces_queue) > 0:
			pieces, path = pieces_queue.popleft()

			if self.PRINT_BOARD_EVERY_PATH:
				self.print_board(pieces)

			self.update_finished(pieces)

			if self.finished:
				print(
					f"\nA shortest path of {len(path)} moves was found!\n"
					f"{self.state_count} unique states were seen.\n"
					"The remaining queue length is {len(pieces_queue)}."
				)

				path_string = "".join(path)
				print(f"Path: {path_string}")

				break

			for piece_label, piece in pieces.items():
				piece_pos = piece["pos"]

				# Saves the position of the piece for when it needs to be moved back.
				x = piece_pos["x"]
				y = piece_pos["y"]

				for direction in range(4):
					self.move(direction, piece_pos)

					if self.is_valid_move(piece_label, piece_pos, pieces):
						new_pieces_positions = self.deepcopy_pieces_positions(pieces)

						new_path_part = piece_label + self.DIRECTION_CHARACTERS[direction]

						pieces_queue.append([ new_pieces_positions, path + [new_path_part] ])

					# Moves the piece back.
					piece_pos["x"] = x
					piece_pos["y"] = y

		self.running = False


	def timed_print(self, pieces_queue):
		if self.running:
			threading.Timer(1, self.timed_print, [pieces_queue]).start()

		if not self.finished:
			elapsed_time = int(time.time() - self.START_TIME)

			states_count_diff = self.state_count - self.prev_state_count
			self.prev_state_count = self.state_count

			print(
				f"\rElapsed time: {elapsed_time} seconds"
				f", Unique states: {self.state_count} (+{states_count_diff}/s)"
				f", Queue length: {len(pieces_queue)}",
				end="",
				flush=True
			)


	def update_finished(self, pieces):
		self.finished = True

		for PIECE_LABEL, ENDING_PIECE in self.ENDING_PIECES.items():
			if pieces[PIECE_LABEL] != ENDING_PIECE:
				self.finished = False


	def move(self, direction, piece_pos):
		match direction:
			case 0:
				piece_pos["y"] += -1
			case 1:
				piece_pos["y"] += 1
			case 2:
				piece_pos["x"] += -1
			case 3:
				piece_pos["x"] += 1


	def is_valid_move(self, piece_label, piece_pos, pieces):
		if (
			self.move_doesnt_cross_puzzle_edge(piece_label, piece_pos) and
			self.no_intersection(piece_label, piece_pos, pieces) and
			self.add_new_state(pieces)
		):
			self.state_count += 1
			return True

		return False


	def move_doesnt_cross_puzzle_edge(self, piece_label, piece_pos):
		x = piece_pos["x"]
		y = piece_pos["y"]

		STARTING_PIECE_SIZE = self.STARTING_PIECES_INFO[piece_label]["size"]
		STARTING_PIECE_WIDTH = STARTING_PIECE_SIZE["width"]
		STARTING_PIECE_HEIGHT = STARTING_PIECE_SIZE["height"]

		if y >= 0 and y + (STARTING_PIECE_HEIGHT - 1) < self.HEIGHT and x >= 0 and x + (STARTING_PIECE_WIDTH - 1) < self.WIDTH:
			return True

		return False


	def no_intersection(self, piece_label_1, piece_1_pos, pieces):
		piece_1_x = piece_1_pos["x"]
		piece_1_y = piece_1_pos["y"]

		PIECE_1_SIZE = self.STARTING_PIECES_INFO[piece_label_1]["size"]
		PIECE_1_WIDTH = PIECE_1_SIZE["width"]
		PIECE_1_HEIGHT = PIECE_1_SIZE["height"]

		piece_1_top = piece_1_y
		piece_1_bottom = piece_1_y + PIECE_1_HEIGHT

		piece_1_left = piece_1_x
		piece_1_right = piece_1_x + PIECE_1_WIDTH

		for piece_label_2, piece_2 in pieces.items():
			if piece_label_1 == piece_label_2:
				continue

			piece_2_pos = piece_2["pos"]
			piece_2_x = piece_2_pos["x"]
			piece_2_y = piece_2_pos["y"]

			SIZE_2 = self.STARTING_PIECES_INFO[piece_label_2]["size"]
			PIECE_2_WIDTH = SIZE_2["width"]
			PIECE_2_HEIGHT = SIZE_2["height"]

			piece_2_top = piece_2_y
			piece_2_bottom = piece_2_y + PIECE_2_HEIGHT

			piece_2_left = piece_2_x
			piece_2_right = piece_2_x + PIECE_2_WIDTH

			if (piece_1_right > piece_2_left and
				piece_1_left < piece_2_right and
				piece_1_bottom > piece_2_top and
				piece_1_top < piece_2_bottom):
				return False

		return True


	# TODO: Check if this is actually faster than a regular deepcopy call.
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
