#include "sliding_puzzle_solver.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name)
{
	json puzzle_json = get_puzzle_json(exe_path, puzzle_name);
	initialize_constant_fields(puzzle_json);
	initialize_variable_fields();
	// std::cout << puzzle_json.dump(4) << std::endl;
}

void SlidingPuzzleSolver::run(void)
{
	std::cout << get_elapsed_seconds().count() << std::endl;
}

////////

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

	this->starting_pieces = json_pieces_to_vector<StartingPiece>(puzzle_json["starting_pieces"]);
	set_starting_pieces_positions();
	this->ending_pieces = json_pieces_to_vector<EndingPiece>(puzzle_json["ending_pieces"]);

	this->start_time = std::chrono::steady_clock::now();

	this->empty_character = ' ';

	this->direction_characters[Direction::UP] = '^';
	this->direction_characters[Direction::DOWN] = 'v';
	this->direction_characters[Direction::LEFT] = '<';
	this->direction_characters[Direction::RIGHT] = '>';
}

template <class T>
std::vector<T> SlidingPuzzleSolver::json_pieces_to_vector(json j)
{
	std::vector<T> v;

	for (json::iterator it = j.begin(); it != j.end(); ++it)
	{
		json pos = (*it)["pos"];
		T p;
		p.pos.x = pos["x"];
		p.pos.y = pos["y"];
		v.push_back(p);
	}

	return v;
}

void set_starting_pieces_positions(void)
{
	// this->starting_pieces_positions
}

void SlidingPuzzleSolver::initialize_variable_fields(void)
{
	this->state_count = 0;
	this->prev_state_count = 0;

	this->running = true;
	this->finished = false;
}

std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - this->start_time;
}

////////

void SlidingPuzzleSolver::print_board(void)
{
	// char board[this->height][this->width] = get_board();

	// for
	// 	row in board : std::cout << row << std::endl;

	// std::cout << std::endl;
}

std::vector<std::vector<char>> SlidingPuzzleSolver::get_board(void)
{
	std::vector<std::vector<char>> board = get_2d_vector();
	return board;
}

std::vector<std::vector<char>> SlidingPuzzleSolver::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(this->height, std::vector<char>(this->width, 0));
}
