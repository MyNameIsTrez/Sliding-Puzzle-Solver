#include "sliding_puzzle_solver.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	const json puzzle_json = get_puzzle_json(exe_path, puzzle_name);
	initialize_constant_fields(puzzle_json);
}

void SlidingPuzzleSolver::run(void)
{
	print_board(starting_pieces);

	add_new_state(starting_pieces);

	solve();
}

const json SlidingPuzzleSolver::get_puzzle_json(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	const std::filesystem::path puzzle_path = get_puzzle_path_from_exe_path(exe_path, puzzle_name);
	std::ifstream stream(puzzle_path);
	const json puzzle_json = json::parse(
		stream,
		nullptr, // callback
		true, // allow exceptions
		true // ignore_comments
	);
	return puzzle_json;
}

const std::filesystem::path SlidingPuzzleSolver::get_puzzle_path_from_exe_path(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	exe_path.remove_filename();

	// TODO: Add the file extension using the filesystem library.
	const std::filesystem::path puzzle_path = exe_path / "puzzles" / (puzzle_name + ".jsonc");
	
	return puzzle_path;
}

void SlidingPuzzleSolver::initialize_constant_fields(const json &puzzle_json)
{
	const json &board_size = puzzle_json["board_size"];
	width = board_size["width"];
	height = board_size["height"];

	const json &starting_pieces_info = puzzle_json["starting_pieces_info"];

	set_starting_pieces_info(starting_pieces_info);
	set_ending_pieces(starting_pieces_info);
	set_starting_pieces();
}

void SlidingPuzzleSolver::set_starting_pieces_info(const json &starting_pieces_info_json)
{
	for (json::const_iterator it = starting_pieces_info_json.cbegin(); it != starting_pieces_info_json.cend(); ++it)
	{
		StartingPieceInfo piece;

		const json &pos = (*it)["pos"];
		piece.pos.x = pos["x"];
		piece.pos.y = pos["y"];

		const json &size = (*it)["size"];
		piece.size.width = size["width"];
		piece.size.height = size["height"];

		starting_pieces_info.push_back(piece);
	}
}

void SlidingPuzzleSolver::set_starting_pieces(void)
{
	for (std::size_t starting_piece_info_index = 0; starting_piece_info_index != starting_pieces_info.size(); ++starting_piece_info_index)
	{
		const StartingPieceInfo &starting_piece_info = starting_pieces_info[starting_piece_info_index];

		Piece starting_piece;
		starting_piece.pos = starting_piece_info.pos;

		starting_pieces.push_back(starting_piece);
	}
}

void SlidingPuzzleSolver::set_ending_pieces(const json &starting_pieces_json)
{
	for (std::size_t starting_piece_json_index = 0; starting_piece_json_index != starting_pieces_json.size(); ++starting_piece_json_index)
	{
		const json &starting_piece_json = starting_pieces_json[starting_piece_json_index];

		if (starting_piece_json.contains("end"))
		{
			EndingPiece ending_piece;
			ending_piece.piece_index = starting_piece_json_index;

			Pos &ending_piece_pos = ending_piece.pos;
			const json &ending_piece_json_pos = starting_piece_json["end"];

			ending_piece_pos.x = ending_piece_json_pos["x"];
			ending_piece_pos.y = ending_piece_json_pos["y"];

			ending_pieces.push_back(ending_piece);
		}
	}
}

template <class T>
void SlidingPuzzleSolver::print_board(const std::vector<T> &pieces)
{
	const std::vector<std::vector<char>> board = get_board(pieces);

	for (std::vector<std::vector<char>>::const_iterator it_row = board.cbegin(); it_row != board.cend(); ++it_row)
	{
		for (std::vector<char>::const_iterator it_chr = it_row->cbegin(); it_chr != it_row->cend(); ++it_chr)
		{
			std::cout << *it_chr;
		}
		std::cout << std::endl;
	}
}

template <class T>
const std::vector<std::vector<char>> SlidingPuzzleSolver::get_board(const std::vector<T> &pieces)
{
	std::vector<std::vector<char>> board = get_2d_vector();

	for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
	{
		const T &piece = pieces[piece_index];
		const Pos &pos = piece.pos;
		const Size &size = starting_pieces_info[piece_index].size;

		const int y = pos.y;
		const int x = pos.x;
		
		const int width = size.width;
		const int height = size.height;

		for (int y2 = y; y2 < y + height; ++y2)
		{
			for (int x2 = x; x2 < x + width; ++x2)
			{
				// TODO: Support more than 26 pieces.
				board[y2][x2] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[piece_index % 26];
			}
		}
	}

	return board;
}

const std::vector<std::vector<char>> SlidingPuzzleSolver::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(height, std::vector<char>(width, empty_character));
}

bool SlidingPuzzleSolver::add_new_state(const std::vector<Piece> &pieces)
{
	const std::size_t hash = get_pieces_hash(pieces);

	const std::pair<std::unordered_set<std::size_t>::iterator, bool> insert_info = states.insert(hash);

	const bool success = insert_info.second;

	return success;
}

void SlidingPuzzleSolver::solve(void)
{
	std::queue<std::vector<Piece>> pieces_queue;
	pieces_queue.push(starting_pieces);
	
	std::queue<std::vector<std::pair<std::size_t, char>>> path_queue;
	const std::vector<std::pair<std::size_t, char>> initial_empty_path = std::vector<std::pair<std::size_t, char>>();
	path_queue.push(initial_empty_path);

	// TODO: Can this be shortened?
	std::thread timed_print_thread(&SlidingPuzzleSolver::timed_print, this, std::ref(pieces_queue));

	while (!pieces_queue.empty())
	{
		std::vector<Piece> pieces = pieces_queue.front();
		pieces_queue.pop();

		std::vector<std::pair<std::size_t, char>> path = path_queue.front();
		path_queue.pop();

		if (print_board_every_path)
			print_board(pieces);

		update_finished(pieces);

		if (finished)
		{
			std::cout << std::endl << std::endl;
			std::cout << "A shortest path of " << path.size() << " moves was found!" << std::endl;
			std::cout << state_count << " unique states were seen." << std::endl;
			std::cout << "The remaining queue length is " << pieces_queue.size() << "." << std::endl;
			
			std::cout << "Path: " << get_path_string(path) << std::endl << std::endl;
			
			break;
		}

		for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
		{
			Piece &piece = pieces[piece_index];
			Pos &piece_pos = piece.pos;

			// Saves the position of the piece for when it needs to be moved back.
			const int x = piece_pos.x;
			const int y = piece_pos.y;

			for (int direction = 0; direction < 4; ++direction)
			{
				// TODO: Is making this an enum that is iterated over faster?
				move(direction, piece_pos);

				if (is_valid_move(piece_index, piece_pos, pieces))
				{
					const std::vector<Piece> new_pieces_positions = deepcopy_pieces_positions(pieces);

					pieces_queue.push(new_pieces_positions);

					std::vector<std::pair<std::size_t, char>> path_copy = path;

					std::pair<std::size_t, char> new_path_part(piece_index, direction_characters[direction]);
					path_copy.push_back(new_path_part);

					path_queue.push(path_copy);

					state_count++;
				}

				// Moves the piece back.
				piece.pos.x = x;
				piece.pos.y = y;
			}
		}
	}

	timed_print_thread.join();
}

void SlidingPuzzleSolver::timed_print(const std::queue<std::vector<Piece>> &pieces_queue)
{
	// TODO: Try to get rid of either the running or finished field.
	// The reason we have them both right now has to do with the last 
	while (!finished)
	{
		// TODO: Store elapsed_time in something more appropriate than int.
		const int elapsed_time = get_elapsed_seconds().count();

		const int states_count_diff = state_count - prev_state_count;
		prev_state_count = state_count;

		std::cout << "\rElapsed time: " << elapsed_time << " seconds";
		std::cout << ", Unique states: " << state_count << " (+" << states_count_diff << "/s)";
		std::cout << ", Queue length: " << pieces_queue.size();
		std::cout << std::flush;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - start_time;
}

void SlidingPuzzleSolver::update_finished(std::vector<Piece> &pieces)
{
	finished = true;

	for (std::size_t ending_piece_vector_index = 0; ending_piece_vector_index != ending_pieces.size(); ++ending_piece_vector_index)
	{
		const EndingPiece &ending_piece = ending_pieces[ending_piece_vector_index];
		const std::size_t ending_piece_index = ending_piece.piece_index;
		const Pos &ending_piece_pos = ending_piece.pos;

		const Piece &piece = pieces[ending_piece_index];
		const Pos &piece_pos = piece.pos;

		// TODO: Use a friend declared operator != to do pos comparison?
		if (ending_piece_pos.x != piece_pos.x || ending_piece_pos.y != piece_pos.y)
			finished = false;
	}
}

void SlidingPuzzleSolver::move(const int direction, Pos &piece_pos)
{
	switch (direction)
	{
	case 0:
		piece_pos.y += -1;
		break;
	case 1:
		piece_pos.y += 1;
		break;
	case 2:
		piece_pos.x += -1;
		break;
	case 3:
		piece_pos.x += 1;
		break;
	}
}

bool SlidingPuzzleSolver::is_valid_move(const std::size_t piece_index, const Pos &piece_pos, const std::vector<Piece> &pieces)
{
	return move_doesnt_cross_puzzle_edge(piece_index, piece_pos) && no_intersection(piece_index, piece_pos, pieces) && add_new_state(pieces);
}

bool SlidingPuzzleSolver::move_doesnt_cross_puzzle_edge(const std::size_t piece_index, const Pos &piece_pos)
{
	const int x = piece_pos.x;
	const int y = piece_pos.y;

	const Size &starting_piece_size = starting_pieces_info[piece_index].size;
	const int starting_piece_width = starting_piece_size.width;
	const int starting_piece_height = starting_piece_size.height;

	return y >= 0 && y + (starting_piece_height - 1) < height && x >= 0 && x + (starting_piece_width - 1) < width;
}

bool SlidingPuzzleSolver::no_intersection(const std::size_t piece_index_1, const Pos &piece_1_pos, const std::vector<Piece> &pieces)
{
	const int piece_1_x = piece_1_pos.x;
	const int piece_1_y = piece_1_pos.y;

	const Size &piece_1_size = starting_pieces_info[piece_index_1].size;
	const int piece_1_width = piece_1_size.width;
	const int piece_1_height = piece_1_size.height;

	const int piece_1_top = piece_1_y;
	const int piece_1_bottom = piece_1_y + piece_1_height;

	const int piece_1_left = piece_1_x;
	const int piece_1_right = piece_1_x + piece_1_width;

	for (std::size_t piece_index_2 = 0; piece_index_2 != pieces.size(); ++piece_index_2)
	{
		if (piece_index_1 == piece_index_2)
			continue;

		const Piece &piece_2 = pieces[piece_index_2];

		const Pos &piece_2_pos = piece_2.pos;
		const int piece_2_x = piece_2_pos.x;
		const int piece_2_y = piece_2_pos.y;

		const Size &size_2 = starting_pieces_info[piece_index_2].size;
		const int piece_2_width = size_2.width;
		const int piece_2_height = size_2.height;

		const int piece_2_top = piece_2_y;
		const int piece_2_bottom = piece_2_y + piece_2_height;

		const int piece_2_left = piece_2_x;
		const int piece_2_right = piece_2_x + piece_2_width;

		if (piece_1_right > piece_2_left &&
			piece_1_left < piece_2_right &&
			piece_1_bottom > piece_2_top &&
			piece_1_top < piece_2_bottom)
			return false;
	}

	return true;
}

// TODO: Check if this is actually faster than a regular deepcopy call.
const std::vector<Piece> SlidingPuzzleSolver::deepcopy_pieces_positions(const std::vector<Piece> &pieces)
{
	std::vector<Piece> deepcopied_pieces_positions;

	for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
	{
		const Piece &piece = pieces[piece_index];

		const Pos &piece_pos = piece.pos;

		Piece deepcopied_piece;
		Pos &deepcopied_piece_position = deepcopied_piece.pos;
		deepcopied_piece_position.x = piece_pos.x;
		deepcopied_piece_position.y = piece_pos.y;

		deepcopied_pieces_positions.push_back(deepcopied_piece);
	}

	return deepcopied_pieces_positions;
}

const std::string SlidingPuzzleSolver::get_path_string(const std::vector<std::pair<std::size_t, char>> &path)
{
	// If this method ever needs to be called a lot then rewrite this to use a string stream/rope instead of += for concatenation.
	std::string path_string;

	for (std::vector<std::pair<std::size_t, char>>::const_iterator pair_it = path.cbegin(); pair_it != path.cend(); ++pair_it)
	{
		std::size_t piece_index = pair_it->first;
		path_string += piece_index;

		char direction = pair_it->second;
		path_string += direction;
	}

	return path_string;
}

// Stolen from here: https://stackoverflow.com/a/27216842
std::size_t SlidingPuzzleSolver::get_pieces_hash(const std::vector<Piece> &pieces) const
{
	std::size_t seed = pieces.size();

	for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
	{
		const Piece &piece = pieces[piece_index];
		const Pos &piece_pos = piece.pos;
		const int x = piece_pos.x;
		const int y = piece_pos.y;
		
		// TODO: Profile whether calculating and storing width instead is faster.
		// const int piece_index = x + y * width;

		seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= y + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	return seed;
}
