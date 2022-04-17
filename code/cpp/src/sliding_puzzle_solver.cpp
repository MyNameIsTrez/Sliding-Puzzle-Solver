#include "sliding_puzzle_solver.hpp"

////////

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	const json puzzle_json = get_puzzle_json(exe_path, puzzle_name);
	initialize_constant_fields(puzzle_json);
}

void SlidingPuzzleSolver::run(void)
{
	print_board();

	add_new_state();

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
	set_starting_pieces_info(puzzle_json["starting_pieces_info"]);
	set_ending_pieces(puzzle_json["starting_pieces_info"]);

	set_width_and_height(puzzle_json["walls"]);
	set_wall_cells(puzzle_json["walls"]);
	set_piece_cells();
}

void SlidingPuzzleSolver::set_starting_pieces_info(const json &starting_pieces_info_json)
{
	// TODO: Replace with const auto & for-loop.
	for (json::const_iterator starting_piece_info_json_iterator = starting_pieces_info_json.cbegin(); starting_piece_info_json_iterator != starting_pieces_info_json.cend(); ++starting_piece_info_json_iterator)
	{
		StartingPieceInfo piece;

		const json &json_top_left = (*starting_piece_info_json_iterator)["top_left"];
		piece.top_left.x = json_top_left["x"];
		piece.top_left.y = json_top_left["y"];

		const json &json_rects = (*starting_piece_info_json_iterator)["rects"];

		// TODO: Replace with const auto & for-loop.
		for (json::const_iterator json_rect_iterator = json_rects.cbegin(); json_rect_iterator != json_rects.cend(); ++json_rect_iterator)
		{
			Rect rect;

			const json &json_rect_offset = (*json_rect_iterator)["offset"];
			rect.offset.x = json_rect_offset["x"];
			rect.offset.y = json_rect_offset["y"];

			const json &json_rect_size = (*json_rect_iterator)["size"];
			rect.size.width = json_rect_size["width"];
			rect.size.height = json_rect_size["height"];

			piece.rects.push_back(rect);
		}

		starting_pieces_info.push_back(piece);
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

			Pos &ending_piece_top_left = ending_piece.top_left;
			const json &ending_piece_json_pos = starting_piece_json["end"];

			ending_piece_top_left.x = ending_piece_json_pos["x"];
			ending_piece_top_left.y = ending_piece_json_pos["y"];

			ending_pieces.push_back(ending_piece);
		}
	}
}

void SlidingPuzzleSolver::set_width_and_height(const json &walls_json)
{
	// Taking the width and height of walls into account is only necessary when the right or bottom wall is two cells thick.
	// I can't think of a reason why they would ever be this thick since those outer wall cells wouldn't influence the puzzle.
	// But hey, making this function handle that edge case (hah) correctly with "x + wall_width" is less puzzling than "x + 1" anyways!

	width = 0;
	height = 0;

	for (const auto &wall_json : walls_json)
	{
		const json &wall_pos_json = wall_json["pos"];
		const json &wall_size_json = wall_json["size"];

		const int x = wall_pos_json["x"];
		const int wall_width = wall_size_json["width"];
		if (x + wall_width > width) width = x + wall_width;

		const int y = wall_pos_json["y"];
		const int wall_height = wall_size_json["height"];
		if (y + wall_height > height) height = y + wall_height;
	}
}

void SlidingPuzzleSolver::set_wall_cells(const json &walls_json)
{
	for (const auto &wall_json : walls_json)
	{
		Wall wall;

		const json &wall_pos_json = wall_json["pos"];
		wall.pos.x = wall_pos_json["x"];
		wall.pos.y = wall_pos_json["y"];

		const json &wall_size_json = wall_json["size"];
		wall.size.width = wall_size_json["width"];
		wall.size.height = wall_size_json["height"];

		for (int y_offset = 0; y_offset < wall.size.height; ++y_offset)
		{
			for (int x_offset = 0; x_offset < wall.size.width; ++x_offset)
			{
				cells[wall.pos.y + y_offset][wall.pos.x + x_offset] = wall_cell_id;
			}
		}
	}
}

void SlidingPuzzleSolver::set_piece_cells(void)
{
	for (cell_id starting_piece_info_index = 0; starting_piece_info_index != static_cast<cell_id>(starting_pieces_info.size()); ++starting_piece_info_index)
	{
		const StartingPieceInfo &starting_piece_info = starting_pieces_info[starting_piece_info_index];
		const Pos &top_left = starting_piece_info.top_left;

		const std::vector<Rect> &rects = starting_piece_info.rects;
		for (const auto &rect : rects)
		{
			const Offset &rect_offset = rect.offset;

			const int rect_top_left_x = top_left.x + rect_offset.x;
			const int rect_top_left_y = top_left.y + rect_offset.y;

			const Size &rect_size = rect.size;

			for (int y_offset = 0; y_offset < rect_size.height; ++y_offset)
			{
				for (int x_offset = 0; x_offset < rect_size.width; ++x_offset)
				{
					cells[rect_top_left_y + y_offset][rect_top_left_x + x_offset] = starting_piece_info_index;
				}
			}
		}
	}
}

void SlidingPuzzleSolver::print_board()
{
	const std::vector<std::vector<char>> board = get_board();

	for (std::vector<std::vector<char>>::const_iterator it_row = board.cbegin(); it_row != board.cend(); ++it_row)
	{
		for (std::vector<char>::const_iterator it_chr = it_row->cbegin(); it_chr != it_row->cend(); ++it_chr)
		{
			std::cout << *it_chr;
		}
		std::cout << std::endl;
	}
}

const std::vector<std::vector<char>> SlidingPuzzleSolver::get_board()
{
	std::vector<std::vector<char>> board = get_2d_vector();

	// TODO: Replace these function calls with a 2D loop going through all the cell_ids in the cells field.
	// set_pieces_on_board(board);
	// set_walls_on_board(board);

	return board;
}

const std::vector<std::vector<char>> SlidingPuzzleSolver::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(height, std::vector<char>(width, empty_character));
}

// void SlidingPuzzleSolver::set_pieces_on_board(std::vector<std::vector<char>> &board)
// {
// 	for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
// 	{
// 		const Piece &piece = pieces[piece_index];

// 		const Pos &top_left = piece.top_left;
// 		const int top_left_x = top_left.x;
// 		const int top_left_y = top_left.y;

// 		const StartingPieceInfo &starting_piece_info = starting_pieces_info[piece_index];
// 		const std::vector<Rect> &rects = starting_piece_info.rects;

// 		for (const auto &rect : rects)
// 		{
// 			const Offset &rect_offset = rect.offset;
// 			const int top_left_rect_x = top_left_x + rect_offset.x;
// 			const int top_left_rect_y = top_left_y + rect_offset.y;

// 			const Size &rect_size = rect.size;
// 			const int rect_width = rect_size.width;
// 			const int rect_height = rect_size.height;

// 			for (int y = top_left_rect_y; y < top_left_rect_y + rect_height; ++y)
// 			{
// 				for (int x = top_left_rect_x; x < top_left_rect_x + rect_width; ++x)
// 				{
// 					board[y][x] = piece_labels[piece_index % piece_labels_length];
// 				}
// 			}
// 		}
// 	}
// }

// void SlidingPuzzleSolver::set_walls_on_board(std::vector<std::vector<char>> &board)
// {
// 	for (const auto &wall : walls)
// 	{
// 		const Pos &wall_pos = wall.pos;
// 		const int top_left_wall_x = wall_pos.x;
// 		const int top_left_wall_y = wall_pos.y;

// 		const Size &wall_size = wall.size;

// 		for (int y = top_left_wall_y; y < top_left_wall_y + wall_size.height; ++y)
// 		{
// 			for (int x = top_left_wall_x; x < top_left_wall_x + wall_size.width; ++x)
// 			{
// 				board[y][x] = wall_character;
// 			}
// 		}
// 	}
// }

bool SlidingPuzzleSolver::add_new_state(void)
{
	const std::pair<std::unordered_set<std::vector<Piece>, Piece::Hasher>::iterator, bool> insert_info = states.insert(pieces);

	const bool success = insert_info.second;

	return success;
}

void SlidingPuzzleSolver::solve(void)
{
	std::stack<std::tuple<cell_id, piece_direction, cell_id, piece_direction>> pieces_stack;
	pieces_stack.push(std::make_tuple(
		NO_RECOVERY, // Recovery piece index.
		NO_RECOVERY, // Recovery direction.
		0, // Piece index.
		0 // Direction.
	));

	// std::stack<std::vector<std::pair<std::size_t, char>>> path_stack;
	// const std::vector<std::pair<std::size_t, char>> initial_empty_path = std::vector<std::pair<std::size_t, char>>();
	// path_stack.push(initial_empty_path);

	// TODO: Can this line be shortened?
	// std::thread timed_print_thread(&SlidingPuzzleSolver::timed_print, this, std::ref(path_stack), std::ref(pieces_stack));

	while (!pieces_stack.empty())
	{
		update_finished();
		if (finished)
		{
			break;
		}

		// TODO: Refactor this using "auto [a, b, c, d]" from C++17.
		std::tuple<cell_id, piece_direction, cell_id, piece_direction> pieces_stack_top = pieces_stack.top();

		const cell_id &recovery_piece_index = std::get<0>(pieces_stack_top);
		const piece_direction &recovery_direction = std::get<1>(pieces_stack_top);
		if (recovery_piece_index != NO_RECOVERY)
		{
			recover_piece(recovery_piece_index, recovery_direction);
		}

		cell_id &piece_index = std::get<2>(pieces_stack_top);
		piece_direction &direction = std::get<3>(pieces_stack_top);

		// std::vector<std::pair<std::size_t, char>> path = path_stack.top();

		// path_stack.pop(); // Purposely placed *after* the break above, as timed_print() is responsible for printing the final path.

		move_piece(piece_index, direction, pieces_stack);
	}

	// timed_print_thread.join();
}

void SlidingPuzzleSolver::recover_piece(const cell_id &recovery_piece_index, const piece_direction &recovery_direction)
{
	Piece &recovery_piece = pieces[recovery_piece_index];
	move(recovery_piece.top_left, recovery_direction);
}

// void SlidingPuzzleSolver::timed_print(const std::stack<std::vector<std::pair<std::size_t, char>>> &path_stack, const std::stack<std::vector<Piece>> &pieces_stack)
// {
// 	std::cout << std::endl;

// 	while (!finished)
// 	{
// 		std::this_thread::sleep_for(std::chrono::seconds(1));
// 		timed_print_core(path_stack, pieces_stack);
// 	}

// 	std::cout << std::endl << std::endl << "Path:" << std::endl;
// 	std::cout << get_path_string(path_stack.top()) << std::endl << std::endl;
// }

// void SlidingPuzzleSolver::timed_print_core(const std::stack<std::vector<std::pair<std::size_t, char>>> &path_stack, const std::stack<std::vector<Piece>> &pieces_stack)
// {
// 	// TODO: Store elapsed_time in something more appropriate than int.
// 	const int elapsed_time = get_elapsed_seconds().count();

// 	const int states_count_diff = state_count - prev_state_count;
// 	prev_state_count = state_count;

// 	std::cout << "\33[2K\r"; // Clears the line and goes back to the left.

// 	std::cout << "Elapsed time: " << elapsed_time << " seconds";

// 	KiloFormatter kf;

// 	if (path_stack.size() > 0)
// 	{
// 		const std::size_t path_length = path_stack.top().size();
// 		std::cout << ", Path length: " << kf.format(path_length);
// 	}

// 	std::cout << ", Unique states: " << kf.format(state_count) << " (+" << kf.format(states_count_diff) << "/s)";
// 	std::cout << ", Queue length: " << kf.format(pieces_stack.size());
// 	std::cout << std::flush;
// }

// std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
// {
// 	// TODO: Cast the result to seconds in type double, cause idk how this works.
// 	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
// 	return end_time - start_time;
// }

void SlidingPuzzleSolver::update_finished()
{
	finished = true;

	for (const auto &ending_piece : ending_pieces)
	{
		const std::size_t ending_piece_index = ending_piece.piece_index;
		const Pos &ending_piece_top_left = ending_piece.top_left;

		const Piece &piece = pieces[ending_piece_index];
		const Pos &piece_top_left = piece.top_left;

		// TODO: Use a friend declared operator != to do pos comparison instead?
		if (ending_piece_top_left.x != piece_top_left.x || ending_piece_top_left.y != piece_top_left.y)
		{
			finished = false;
			return;
		}
	}
}

void SlidingPuzzleSolver::move_piece(cell_id &piece_index, piece_direction &direction, std::stack<std::tuple<cell_id, piece_direction, cell_id, piece_direction>> &pieces_stack)
{
	for ( ; piece_index != static_cast<cell_id>(pieces.size()); ++piece_index)
	{
		Piece &piece = pieces[piece_index];
		Pos &piece_top_left = piece.top_left;

		const std::vector<Rect> &rects = starting_pieces_info[piece_index].rects;

		const int piece_top_left_x_backup = piece_top_left.x;
		const int piece_top_left_y_backup = piece_top_left.y;

		for ( ; direction < 4; ++direction)
		{
			if (a_rect_cant_be_moved(rects, direction, piece_index, piece_top_left))
			{
				continue;
			}

			move(piece_top_left, direction);

			if (add_new_state())
			{
				pieces_stack.push(std::make_tuple(
					piece_index, // Recovery piece index.
					get_inverted_direction(direction), // Recovery direction.
					get_next_piece_index(piece_index, direction),
					get_next_direction(piece_index, direction)
				));

				// std::vector<std::pair<std::size_t, char>> path_copy = path;

				// std::pair<std::size_t, char> new_path_part(piece_index, direction_characters[direction]);
				// path_copy.push_back(new_path_part);

				// path_stack.push(path_copy);

				state_count++;
			}

			// TODO: This may be unnecessary here?
			piece_top_left.x = piece_top_left_x_backup;
			piece_top_left.y = piece_top_left_y_backup;
		}
	}

	// If no piece could be moved, then recurse back.
	pieces_stack.pop();
}

bool SlidingPuzzleSolver::a_rect_cant_be_moved(const std::vector<Rect> &rects, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left)
{
	for (const auto &rect : rects)
	{
		if (is_invalid_move(rect, direction, piece_id, piece_top_left))
		{
			return true;
		}
	}

	return false;
}

bool SlidingPuzzleSolver::is_invalid_move(const Rect &rect, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left)
{
	/*
	Only the cells on the edges are checked for a collision, in this numbered order:
	11111
	3   4
	3   4
	22222
	*/

	const int rect_left_x = piece_top_left.x + rect.offset.x;
	const int rect_top_y = piece_top_left.y + rect.offset.y;

	const Size &rect_size = rect.size;
	const int rect_width = rect_size.width;
	const int rect_height = rect_size.height;

	const int rect_bottom_y = rect_top_y + rect_height;
	const int rect_right_x = rect_left_x + rect_width;

	switch (direction)
	{
	case 0:
		return is_horizontal_invalid_move(piece_id, rect_left_x, rect_top_y, rect_width);
	case 1:
		return is_horizontal_invalid_move(piece_id, rect_left_x, rect_bottom_y, rect_width);
	case 2:
		return is_vertical_invalid_move(piece_id, rect_left_x, rect_top_y, rect_height);
	default:
		return is_vertical_invalid_move(piece_id, rect_right_x, rect_top_y, rect_height);
	}
}

bool SlidingPuzzleSolver::is_horizontal_invalid_move(const cell_id piece_id, const int start_x, const int start_y, const int rect_width)
{
	for (int x_offset = 0; x_offset < rect_width; ++x_offset)
	{
		if (can_id_be_placed(piece_id, start_x + x_offset, start_y))
		{
			return true;
		}
	}

	return false;
}

bool SlidingPuzzleSolver::is_vertical_invalid_move(const cell_id piece_id, const int start_x, const int start_y, const int rect_height)
{
	for (int y_offset = 0; y_offset < rect_height; ++y_offset)
	{
		if (can_id_be_placed(piece_id, start_x, start_y + y_offset))
		{
			return true;
		}
	}

	return false;
}

bool SlidingPuzzleSolver::can_id_be_placed(const cell_id piece_id, const int x, const int y)
{
	const cell_id checked_cell_id = cells[y][x];

	if (checked_cell_id == empty_cell_id || checked_cell_id == piece_id)
	{
		return true;
	}

	return false;
}

void SlidingPuzzleSolver::move(Pos &piece_top_left, const piece_direction direction)
{
	switch (direction)
	{
	case 0:
		piece_top_left.y += -1;
		break;
	case 1:
		piece_top_left.y += 1;
		break;
	case 2:
		piece_top_left.x += -1;
		break;
	case 3:
		piece_top_left.x += 1;
		break;
	}
}

piece_direction SlidingPuzzleSolver::get_inverted_direction(const piece_direction &direction)
{
	switch (direction)
	{
	case 0:
		return 1;
	case 1:
		return 0;

	case 2:
		return 3;
	default:
		return 2;
	}
}

cell_id SlidingPuzzleSolver::get_next_piece_index(const cell_id &piece_index, const piece_direction &direction)
{
	// TODO: What if this was the last movable piece?
	(void)direction;
	return piece_index + 1;
}

piece_direction SlidingPuzzleSolver::get_next_direction(const cell_id &piece_index, const piece_direction &direction)
{
	// TODO: What if this was the last movable piece?
	(void)piece_index;
	return (direction + 1) % 4;
}

// bool SlidingPuzzleSolver::move_doesnt_cross_puzzle_edge(const std::size_t piece_index, const Pos &piece_top_left)
// {
// 	const int piece_x = piece_top_left.x;
// 	const int piece_y = piece_top_left.y;

// 	const Size &starting_piece_size = starting_pieces_info[piece_index].size;
// 	const int starting_piece_width = starting_piece_size.width;
// 	const int starting_piece_height = starting_piece_size.height;

// 	return (
// 		piece_y >= 1 && piece_y + (starting_piece_height - 1) < height - 1 &&
// 		piece_x >= 1 && piece_x + (starting_piece_width - 1) < width - 1
// 	);
// }

// bool SlidingPuzzleSolver::no_intersection(const std::size_t piece_index_1, const Pos &piece_1_pos)
// {
// 	const int piece_1_x = piece_1_pos.x;
// 	const int piece_1_y = piece_1_pos.y;

// 	const Size &piece_1_size = starting_pieces_info[piece_index_1].size;
// 	const int piece_1_width = piece_1_size.width;
// 	const int piece_1_height = piece_1_size.height;

// 	const int piece_1_top = piece_1_y;
// 	const int piece_1_bottom = piece_1_y + piece_1_height;

// 	const int piece_1_left = piece_1_x;
// 	const int piece_1_right = piece_1_x + piece_1_width;

// 	for (std::size_t piece_index_2 = 0; piece_index_2 != pieces.size(); ++piece_index_2)
// 	{
// 		if (piece_index_1 == piece_index_2)
// 		{
// 			continue;
// 		}

// 		const Piece &piece_2 = pieces[piece_index_2];

// 		const Pos &piece_2_pos = piece_2.pos;
// 		const int piece_2_x = piece_2_pos.x;
// 		const int piece_2_y = piece_2_pos.y;

// 		const Size &size_2 = starting_pieces_info[piece_index_2].size;
// 		const int piece_2_width = size_2.width;
// 		const int piece_2_height = size_2.height;

// 		const int piece_2_top = piece_2_y;
// 		const int piece_2_bottom = piece_2_y + piece_2_height;

// 		const int piece_2_left = piece_2_x;
// 		const int piece_2_right = piece_2_x + piece_2_width;

// 		if (piece_1_right > piece_2_left &&
// 			piece_1_left < piece_2_right &&
// 			piece_1_bottom > piece_2_top &&
// 			piece_1_top < piece_2_bottom)
// 		{
// 			return false;
// 		}
// 	}

// 	return true;
// }

// const std::string SlidingPuzzleSolver::get_path_string(const std::vector<std::pair<std::size_t, char>> &path)
// {
// 	// If this method ever needs to be called a lot then try using a rope instead.
// 	std::stringstream path_stringstream;

// 	for (std::vector<std::pair<std::size_t, char>>::const_iterator pair_it = path.cbegin(); pair_it != path.cend(); ++pair_it)
// 	{
// 		std::size_t piece_index = pair_it->first;
// 		path_stringstream << piece_index;

// 		char direction = pair_it->second;
// 		path_stringstream << direction;
// 	}

// 	return path_stringstream.str();
// }
