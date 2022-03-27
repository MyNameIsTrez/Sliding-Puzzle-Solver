from sliding_puzzle_solver import SlidingPuzzleSolver

from puzzles.klotski import Klotski


def main():
	sliding_puzzle_solver = SlidingPuzzleSolver(Klotski())
	sliding_puzzle_solver.run()


if __name__ == "__main__":
	main()
