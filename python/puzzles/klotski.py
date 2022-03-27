class Klotski:
	def __init__(self):
		self.BOARD_SIZE = {
			"WIDTH": 4,
			"HEIGHT": 5
		}

		# ABBC
		# ABBC
		# DEEF
		# DGHF
		# I  J
		self.PIECES = {
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

		self.PIECE_ENDING_POSITIONS = {
			"B": { "x": 1, "y": 3 }
		}
