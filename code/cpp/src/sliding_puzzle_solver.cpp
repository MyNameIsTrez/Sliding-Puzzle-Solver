#include "sliding_puzzle_solver.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name)
{
	json puzzle_json = this->get_puzzle_json(exe_path, puzzle_name);
	this->initialize_constant_fields(puzzle_json);
	this->initialize_variable_fields();
}

void SlidingPuzzleSolver::run(void)
{
	this->print_board(this->starting_pieces);

	this->add_new_state(this->starting_pieces);

	this->solve();

	std::cout << std::endl << "Done!" << std::endl;
}

////////

json SlidingPuzzleSolver::get_puzzle_json(std::filesystem::path exe_path, std::string puzzle_name)
{
	std::filesystem::path puzzle_path = this->get_puzzle_path_from_exe_path(exe_path, puzzle_name);
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

	this->starting_pieces_info = this->json_starting_piece_info_to_map(puzzle_json["starting_pieces_info"]);
	this->set_starting_pieces();
	this->ending_pieces = this->json_ending_piece_to_map(puzzle_json["ending_pieces"]);

	this->start_time = std::chrono::steady_clock::now();

	this->empty_character = ' ';

	this->direction_characters = {'^', 'v', '<', '>'};

	this->print_board_every_path = false;
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

////////

template <class T>
void SlidingPuzzleSolver::print_board(std::map<std::string, T> pieces)
{
	std::vector<std::vector<char>> board = this->get_board(pieces);

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
	std::vector<std::vector<char>> board = this->get_2d_vector();

	// TODO: Figure out if "typename" here is really necessary and why if it is.
	for (typename std::map<std::string, T>::const_iterator it = pieces.begin(); it != pieces.end(); ++it)
	{
		std::string piece_label = it->first;
		Pos pos = it->second.pos;
		Size size = this->starting_pieces_info[piece_label].size;

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
	std::queue<std::map<std::string, Piece>> pieces_queue;

	pieces_queue.push(this->starting_pieces);

	// std::thread timed_print_thread(this->timed_print);
	std::thread timed_print_thread(&SlidingPuzzleSolver::timed_print, this, std::ref(pieces_queue));

	while (!pieces_queue.empty())
	{
		std::map<std::string, Piece> pieces = pieces_queue.front();
		pieces_queue.pop();

		if (this->print_board_every_path)
			this->print_board(pieces);

		this->update_finished(pieces);

		if (this->finished)
		{
			std::cout << std::endl << std::endl;
			std::cout << "A shortest path of " << " moves was found!" << std::endl;
			std::cout << this->state_count << " unique states were seen." << std::endl;
			std::cout << "The remaining queue length is " << pieces_queue.size() << "." << std::endl;
			break;
		}

		for (std::map<std::string, Piece>::iterator piece_it = pieces.begin(); piece_it != pieces.end(); ++piece_it)
		{
			std::string piece_label = piece_it->first;
			Piece &piece = piece_it->second;

			Pos &piece_pos = piece.pos;

			// Saves the position of the piece for when it needs to be moved back.
			int x = piece_pos.x;
			int y = piece_pos.y;

			for (int direction = 0; direction < 4; ++direction)
			{
				// TODO: Is making this an enum that is iterated over faster?
				this->move(direction, piece_pos);

				if (this->is_valid_move(piece_label, piece_pos, pieces))
				{
					std::map<std::string, Piece> new_pieces_positions = this->deepcopy_pieces_positions(pieces);

					pieces_queue.push(new_pieces_positions);
				}

				// Moves the piece back.
				piece.pos.x = x;
				piece.pos.y = y;
			}
		}
	}

	this->running = false;
	timed_print_thread.join();
}

void SlidingPuzzleSolver::timed_print(std::queue<std::map<std::string, Piece>> &pieces_queue)
{
	while (this->running)
	{
		if (!this->finished)
		{
			// TODO: Store elapsed_time in something more appropriate than int.
			int elapsed_time = this->get_elapsed_seconds().count();

			int states_count_diff = this->state_count - this->prev_state_count;
			this->prev_state_count = this->state_count;

			std::cout << "\rElapsed time: " << elapsed_time << " seconds";
			std::cout << ", Unique states: " << this->state_count << " (+" << states_count_diff << "/s)";
			std::cout << ", Queue length: " << pieces_queue.size();
			std::cout << std::flush;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - this->start_time;
}

void SlidingPuzzleSolver::update_finished(std::map<std::string, Piece> pieces)
{
	this->finished = true;

	for (std::map<std::string, Piece>::iterator ending_piece_it = this->ending_pieces.begin(); ending_piece_it != this->ending_pieces.end(); ++ending_piece_it)
	{
		std::string piece_label = ending_piece_it->first;
		Piece ending_piece = ending_piece_it->second;

		if (pieces[piece_label] != ending_piece)
			this->finished = false;
	}
}

void SlidingPuzzleSolver::move(int direction, Pos &piece_pos)
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

bool SlidingPuzzleSolver::is_valid_move(std::string piece_label, Pos piece_pos, std::map<std::string, Piece> pieces)
{
	if (this->move_doesnt_cross_puzzle_edge(piece_label, piece_pos) &&
		this->no_intersection(piece_label, piece_pos, pieces) &&
		this->add_new_state(pieces))
	{
		this->state_count += 1;
		return true;
	}

	return false;
}

bool SlidingPuzzleSolver::move_doesnt_cross_puzzle_edge(std::string piece_label, Pos piece_pos)
{
	int x = piece_pos.x;
	int y = piece_pos.y;

	Size starting_piece_size = this->starting_pieces_info[piece_label].size;
	int starting_piece_width = starting_piece_size.width;
	int starting_piece_height = starting_piece_size.height;

	if (y >= 0 && y + (starting_piece_height - 1) < this->height && x >= 0 && x + (starting_piece_width - 1) < this->width)
		return true;

	return false;
}

bool SlidingPuzzleSolver::no_intersection(std::string piece_label_1, Pos piece_1_pos, std::map<std::string, Piece> pieces)
{
	int piece_1_x = piece_1_pos.x;
	int piece_1_y = piece_1_pos.y;

	Size piece_1_size = this->starting_pieces_info[piece_label_1].size;
	int piece_1_width = piece_1_size.width;
	int piece_1_height = piece_1_size.height;

	int piece_1_top = piece_1_y;
	int piece_1_bottom = piece_1_y + piece_1_height;

	int piece_1_left = piece_1_x;
	int piece_1_right = piece_1_x + piece_1_width;

	for (std::map<std::string, Piece>::const_iterator it = pieces.begin(); it != pieces.end(); ++it)
	{
		std::string piece_label_2 = it->first;
		Piece piece_2 = it->second;

		if (piece_label_1 == piece_label_2)
			continue;

		Pos piece_2_pos = piece_2.pos;
		int piece_2_x = piece_2_pos.x;
		int piece_2_y = piece_2_pos.y;

		Size size_2 = this->starting_pieces_info[piece_label_2].size;
		int piece_2_width = size_2.width;
		int piece_2_height = size_2.height;

		int piece_2_top = piece_2_y;
		int piece_2_bottom = piece_2_y + piece_2_height;

		int piece_2_left = piece_2_x;
		int piece_2_right = piece_2_x + piece_2_width;

		if (piece_1_right > piece_2_left &&
			piece_1_left < piece_2_right &&
			piece_1_bottom > piece_2_top &&
			piece_1_top < piece_2_bottom)
			return false;
	}

	return true;
}

// TODO: Check if this is actually faster than a regular deepcopy call.
std::map<std::string, Piece> SlidingPuzzleSolver::deepcopy_pieces_positions(std::map<std::string, Piece> pieces)
{
	std::map<std::string, Piece> deepcopied_pieces_positions;

	for (std::map<std::string, Piece>::const_iterator it = pieces.begin(); it != pieces.end(); ++it)
	{
		std::string piece_label = it->first;
		Piece piece = it->second;
		Pos piece_pos = piece.pos;

		Pos &deepcopied_piece_position = deepcopied_pieces_positions[piece_label].pos;
		deepcopied_piece_position.x = piece_pos.x;
		deepcopied_piece_position.y = piece_pos.y;
	}

	return deepcopied_pieces_positions;
}
