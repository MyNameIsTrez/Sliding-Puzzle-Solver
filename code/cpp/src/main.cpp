#include "main.hpp"

////////

int main(int argc, char *argv[])
{
	(void)argc;

	std::filesystem::path exe_path = argv[0];

	SlidingPuzzleSolver sliding_puzzle_solver(exe_path, "klotski");

	return EXIT_SUCCESS;
}
