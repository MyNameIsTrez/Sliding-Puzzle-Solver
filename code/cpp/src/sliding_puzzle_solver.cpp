#include "main.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name)
{
	json puzzle_json = get_puzzle_json(exe_path, puzzle_name);
	initialize_constant_fields(puzzle_json);
	initialize_variable_fields();
	std::cout << puzzle_json.dump(4) << std::endl;
}

void SlidingPuzzleSolver::run(void)
{
}

json SlidingPuzzleSolver::get_puzzle_json(std::filesystem::path exe_path, std::string puzzle_name)
{
	std::filesystem::path puzzle_path = get_puzzle_path_from_exe_path(exe_path, puzzle_name);
	std::ifstream f(puzzle_path);
	json puzzle_json = json::parse(f,
								   /* callback */ nullptr,
								   /* allow exceptions */ true,
								   /* ignore_comments */ true);
	return puzzle_json;
}

std::filesystem::path SlidingPuzzleSolver::get_puzzle_path_from_exe_path(std::filesystem::path exe_path, std::string puzzle_name)
{
	std::filesystem::path exe_dir_path = exe_path.remove_filename();
	std::filesystem::path puzzle_path = exe_dir_path / "puzzles" / (puzzle_name + ".jsonc"); // TODO: filesystem way of adding extension
	return puzzle_path;
}

void SlidingPuzzleSolver::initialize_constant_fields(json puzzle_json)
{
	json board_size = puzzle_json["board_size"];
	this->width = board_size["width"];
	this->height = board_size["height"];

	this->pieces = puzzle_json["pieces"];
	this->piece_ending_positions = puzzle_json["piece_ending_positions"];

	this->start_time = std::chrono::steady_clock::now();

	this->empty_character = ' ';

	this->direction_characters[Direction::UP] = '^';
	this->direction_characters[Direction::DOWN] = 'v';
	this->direction_characters[Direction::LEFT] = '<';
	this->direction_characters[Direction::RIGHT] = '>';
}

void SlidingPuzzleSolver::initialize_variable_fields(void)
{
	// this->states = std::vector<>;
}
