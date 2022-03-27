#include "main.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name)
{
	std::filesystem::path puzzle_path = get_puzzle_path_from_exe_path(exe_path, puzzle_name);
	json j = get_puzzle_json(puzzle_path);
	std::cout << j.dump(4) << std::endl;
}

std::filesystem::path SlidingPuzzleSolver::get_puzzle_path_from_exe_path(std::filesystem::path exe_path, std::string puzzle_name)
{
	std::filesystem::path exe_dir_path = exe_path.remove_filename();
	std::filesystem::path puzzle_path = exe_dir_path / "puzzles" / (puzzle_name + ".jsonc"); // TODO: filesystem way of adding extension
	return puzzle_path;
}

json SlidingPuzzleSolver::get_puzzle_json(std::filesystem::path puzzle_path)
{
	std::ifstream f(puzzle_path);
	json j = json::parse(f,
						 /* callback */ nullptr,
						 /* allow exceptions */ true,
						 /* ignore_comments */ true);
	return j;
}

void SlidingPuzzleSolver::Run(void)
{
}
