CHOSEN_PUZZLE = "klotski"


PUZZLES = {
	"klotski": {
		"BOARD_SIZE": {
			"width": 4,
			"height": 5
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


PUZZLE = PUZZLES[CHOSEN_PUZZLE]

BOARD_SIZE = PUZZLE["BOARD_SIZE"]
PIECES = PUZZLE["PIECES"]


def get_board():
	board = [[" "] * BOARD_SIZE["width"] for _ in range(BOARD_SIZE["height"])]

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

	for row in board:
		print(row)


def main():
	print_board()

	# print(BOARD_SIZE, PIECES)


if __name__ == "__main__":
	main()