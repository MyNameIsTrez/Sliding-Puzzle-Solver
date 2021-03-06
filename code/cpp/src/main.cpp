#include "sliding_puzzle_solver.hpp"

////////

int main(int argc, char *argv[])
{
	(void)argc;

	std::filesystem::path exe_path = argv[0];

	SlidingPuzzleSolver sliding_puzzle_solver(exe_path, "klotski");

	sliding_puzzle_solver.solve();

	return EXIT_SUCCESS;
}
