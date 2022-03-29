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
	print_board(this->starting_pieces);

	add_new_state(this->starting_pieces);

	solve();

	std::cout << "\nDone!" << std::endl;

	// std::cout << get_elapsed_seconds().count() << std::endl;
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
	// TODO: Add the file extension using the filesystem library.
	std::filesystem::path puzzle_path = exe_dir_path / "puzzles" / (puzzle_name + ".jsonc");
	return puzzle_path;
}

void SlidingPuzzleSolver::initialize_constant_fields(json puzzle_json)
{
	json board_size = puzzle_json["board_size"];
	this->width = board_size["width"];
	this->height = board_size["height"];

	this->starting_pieces_info = json_starting_piece_info_to_map(puzzle_json["starting_pieces_info"]);
	set_starting_pieces();
	this->ending_pieces = json_ending_piece_to_map(puzzle_json["ending_pieces"]);

	this->start_time = std::chrono::steady_clock::now();

	this->empty_character = ' ';

	this->direction_characters[Direction::UP] = '^';
	this->direction_characters[Direction::DOWN] = 'v';
	this->direction_characters[Direction::LEFT] = '<';
	this->direction_characters[Direction::RIGHT] = '>';
}

std::map<std::string, StartingPieceInfo> SlidingPuzzleSolver::json_starting_piece_info_to_map(json j)
{
	std::map<std::string, StartingPieceInfo> m;

	// TODO: Can const_iterator be used here?
	for (json::iterator it = j.begin(); it != j.end(); ++it)
	{
		StartingPieceInfo p;

		json pos = (*it)["pos"];
		p.pos.x = pos["x"];
		p.pos.y = pos["y"];

		json size = (*it)["size"];
		p.size.width = size["width"];
		p.size.height = size["height"];

		// TODO: Can .first be used instead of .key()?
		m[it.key()] = p;
	}

	return m;
}

std::map<std::string, Piece> SlidingPuzzleSolver::json_ending_piece_to_map(json j)
{
	std::map<std::string, Piece> m;

	// TODO: Can const_iterator be used here?
	for (json::iterator it = j.begin(); it != j.end(); ++it)
	{
		Piece p;

		json pos = (*it)["pos"];
		p.pos.x = pos["x"];
		p.pos.y = pos["y"];

		// TODO: Can .first be used instead of .key()?
		m[it.key()] = p;
	}

	return m;
}

void SlidingPuzzleSolver::set_starting_pieces(void)
{
	// TODO: Can const_iterator be used here?
	for (std::map<std::string, StartingPieceInfo>::iterator it = this->starting_pieces_info.begin(); it != this->starting_pieces_info.end(); ++it)
	{
		this->starting_pieces[it->first].pos = it->second.pos;
	}
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

template <class T>
void SlidingPuzzleSolver::print_board(std::map<std::string, T> pieces)
{
	std::vector<std::vector<char>> board = get_board(pieces);

	for (std::vector<std::vector<char>>::const_iterator it_row = board.begin(); it_row != board.end(); ++it_row)
	{
		for (std::vector<char>::const_iterator it_chr = it_row->begin(); it_chr != it_row->end(); ++it_chr)
		{
			std::cout << *it_chr;
		}
		std::cout << std::endl;
	}
}

template <class T>
std::vector<std::vector<char>> SlidingPuzzleSolver::get_board(std::map<std::string, T> pieces)
{
	std::vector<std::vector<char>> board = get_2d_vector();

	for (typename std::map<std::string, T>::const_iterator it = pieces.begin(); it != pieces.end(); ++it)
	{
		std::string piece_label = it->first;
		struct Pos pos = it->second.pos;
		struct Size size = this->starting_pieces_info[piece_label].size;

		int y = pos.y;
		int height = size.height;

		for (int y2 = y; y2 < y + height; ++y2)
		{
			int x = pos.x;
			int width = size.width;

			for (int x2 = x; x2 < x + width; ++x2)
			{
				// TODO: Handle the label string to char for printing conversion better.
				board[y2][x2] = piece_label[0];
			}
		}
	}

	return board;
}

std::vector<std::vector<char>> SlidingPuzzleSolver::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(this->height, std::vector<char>(this->width, ' '));
}

bool SlidingPuzzleSolver::add_new_state(std::map<std::string, Piece> pieces)
{
	std::pair<std::set<std::map<std::string, Piece>>::iterator, bool> ret;

	ret = this->states.insert(pieces);

	return ret.second;
}

void SlidingPuzzleSolver::solve(void)
{
	std::queue<std::vector<Piece>> pieces_queue;
	(void)pieces_queue;
}

void SlidingPuzzleSolver::solve_and_print_path(void)
{
	std::queue<std::pair<Piece, std::vector<std::string>>> pieces_queue;
	(void)pieces_queue;
}
