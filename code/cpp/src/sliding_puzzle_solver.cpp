#include "main.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name)
{
	json puzzle_json = get_puzzle_json(exe_path, puzzle_name);
	initialize_constant_fields(puzzle_json);
	initialize_variable_fields();
}

void SlidingPuzzleSolver::run(void)
{
	// std::cout << puzzle_json.dump(4) << std::endl;
	std::cout << get_elapsed_seconds().count() << std::endl;
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
	std::cout << this->states.empty() << std::endl;
	Piece piece;
	piece.pos.x = 4;
	piece.pos.y = 2;
	this->states.push_back(piece);
	std::cout << this->states.empty() << std::endl;
	std::cout << this->states.back().pos.x << std::endl;
	std::cout << this->states.back().pos.x << std::endl;
}

std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - this->start_time;
}
