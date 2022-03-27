import threading, copy, time

from collections import deque
from enum import Enum, auto


class Direction(Enum):
	UP = auto()
	DOWN = auto()
	LEFT = auto()
	RIGHT = auto()


class SlidingPuzzleSolver:
	def __init__(self, puzzle_instance):
		self.initialize_constant_fields(puzzle_instance)
		self.initialize_variable_fields()


	def initialize_constant_fields(self, puzzle_instance):
		self.EMPTY_CHARACTER = " "

		BOARD_SIZE = puzzle_instance.BOARD_SIZE
		self.WIDTH = BOARD_SIZE["WIDTH"]
		self.HEIGHT = BOARD_SIZE["HEIGHT"]

		self.PIECES = puzzle_instance.PIECES

		self.PIECE_ENDING_POSITIONS = puzzle_instance.PIECE_ENDING_POSITIONS

		self.START_TIME = time.time()

		self.DIRECTION_CHARACTERS = {
			Direction.UP: "^",
			Direction.DOWN: "v",
			Direction.LEFT: "<",
			Direction.RIGHT: ">",
		}


	def run(self):
		self.print_board()

		starting_positions = self.get_starting_positions()
		self.add_new_state(starting_positions)
		self.solve(starting_positions)

		print("\nDone!")


	def initialize_variable_fields(self):
		self.states = {}
		self.state_count = 0
		self.prev_state_count = 0

		self.running = True
		self.finished = False


	def get_starting_positions(self):
		starting_positions = {}

		for piece_label, piece in self.PIECES.items():
			starting_positions[piece_label] = {
				"x": piece["pos"]["x"],
				"y": piece["pos"]["y"]
			}

		return starting_positions


	def add_new_state(self, pieces):
		state = self.states

		new_state = False

		for piece in pieces.values():
			x = piece["x"]
			y = piece["y"]

			piece_coordinate_index = x + y * self.WIDTH

			if piece_coordinate_index not in state:
				new_state = True
				state[piece_coordinate_index] = {}
			state = state[piece_coordinate_index]

		return new_state


	def print_board(self):
		board = self.get_board()

		for row in board:
			print(row)

		print()


	def get_board(self):
		board = [[self.EMPTY_CHARACTER] * self.WIDTH for _ in range(self.HEIGHT)]

		for piece_label, piece in self.PIECES.items():
			pos, size = piece["pos"], piece["size"]

			y = pos["y"]
			height = size["height"]

			for y2 in range(y, y + height):
				x = pos["x"]
				width = size["width"]

				for x2 in range(x, x + width):
					board[y2][x2] = piece_label

		return board


	def solve(self, starting_positions):
		queue = deque([ [starting_positions.copy(), []] ])

		self.timed_print(queue)

		while len(queue) > 0:
			pieces_positions, path = queue.popleft()

			self.update_finished(pieces_positions)

			if self.finished:
				finished_message = f"\nA shortest path of {len(path)} moves was found! {state_count} unique states were seen. The remaining queue length is {len(queue)}."
				print(finished_message)

				path_string = "".join(path)
				print(f"Path: {path_string}")

				break

			for piece_label, piece in pieces_positions.items():
				# Saves the position of the piece, in case it needs to be moved back
				x = piece["x"]
				y = piece["y"]

				for direction in Direction:
					if self.is_valid_move(direction, piece_label, piece, pieces_positions):
						new_pieces_positions = self.deepcopy_pieces_positions(pieces_positions)

						new_path_part = piece_label + self.DIRECTION_CHARACTERS[direction]

						queue.append([ new_pieces_positions, path + [new_path_part] ])

						# Moves the piece back
						piece["x"] = x
						piece["y"] = y

		self.running = False


	def update_finished(self, pieces_positions):
		self.finished = True

		for label, piece_ending_position in self.PIECE_ENDING_POSITIONS.items():
			if pieces_positions[label] != piece_ending_position:
				self.finished = False


	def deepcopy_pieces_positions(self, pieces):
		deepcopied_pieces_positions = {}

		for piece_label, piece in pieces.items():
			deepcopied_pieces_positions[piece_label] = {}
			deepcopied_piece_pos = deepcopied_pieces_positions[piece_label]

			deepcopied_piece_pos["x"] = piece["x"]
			deepcopied_piece_pos["y"] = piece["y"]

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
				f", Unique states: {self.state_count} (+{states_count_diff})"
				f", Queue length: {len(queue)}",
				end="",
				flush=True
			)


	def is_valid_move(self, direction, piece_label, piece, pieces):
		# Saves the position of the piece, in case it needs to be moved back
		x = piece["x"]
		y = piece["y"]

		self.move(direction, piece)

		if (
			self.move_doesnt_cross_puzzle_edge(piece_label, piece) and
			self.no_intersection(piece_label, piece, pieces) and
			self.add_new_state(pieces)
		):
			self.state_count += 1
			return True

		# Moves the piece back
		piece["x"] = x
		piece["y"] = y

		return False


	# TODO: Possibly use lookup table for x and y instead if it's faster in C++
	def move(self, direction, piece):
		match direction:
			case Direction.UP:
				piece["y"] += -1
			case Direction.DOWN:
				piece["y"] += 1
			case Direction.LEFT:
				piece["x"] += -1
			case Direction.RIGHT:
				piece["x"] += 1


	def move_doesnt_cross_puzzle_edge(self, piece_label, piece):
		x = piece["x"]
		y = piece["y"]

		PIECE = self.PIECES[piece_label]
		size = PIECE["size"]
		width = size["width"]
		height = size["height"]

		if y >= 0 and y + (height - 1) < self.HEIGHT and x >= 0 and x + (width - 1) < self.WIDTH:
			return True
		return False


	def no_intersection(self, piece_label_1, piece1, pieces):
		size1 = self.PIECES[piece_label_1]["size"]

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

			size2 = self.PIECES[piece_label_2]["size"]

			x2 = piece2["x"]
			y2 = piece2["y"]

			w2 = size2["width"]
			h2 = size2["height"]

			p2t = y2
			p2b = y2 + h2

			p2l = x2
			p2r = x2 + w2

			if p1r > p2l and p1l < p2r and p1b > p2t and p1t < p2b:
				return False

		return True
