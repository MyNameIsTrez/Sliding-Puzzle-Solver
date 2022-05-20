#include "sliding_puzzle_solver.hpp"


SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	const json puzzle_json = get_puzzle_json(exe_path, puzzle_name);

	set_constant_fields(puzzle_json);
	initialize_variable_fields(puzzle_json);

	set_emptied_offsets();
	set_collision_offsets();
}


void SlidingPuzzleSolver::run(void)
{
	// print_board();

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


void SlidingPuzzleSolver::set_constant_fields(const json &puzzle_json)
{
	set_starting_pieces_info(puzzle_json["starting_pieces_info"]);
	set_pieces_count();

	set_ending_pieces(puzzle_json["starting_pieces_info"]);

	set_walls(puzzle_json["walls"]);
	set_width_and_height();
}


void SlidingPuzzleSolver::set_starting_pieces_info(const json &starting_pieces_info_json)
{
	// TODO: Replace with const auto & for-loop.
	for (json::const_iterator starting_piece_info_json_iterator = starting_pieces_info_json.cbegin(); starting_piece_info_json_iterator != starting_pieces_info_json.cend(); ++starting_piece_info_json_iterator)
	{
		StartingPieceInfo starting_piece_info;

		const json &json_top_left = (*starting_piece_info_json_iterator)["top_left"];
		starting_piece_info.top_left.x = json_top_left["x"];
		starting_piece_info.top_left.y = json_top_left["y"];

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

			starting_piece_info.rects.push_back(rect);
		}

		starting_pieces_info.push_back(starting_piece_info);
	}
}


void SlidingPuzzleSolver::set_pieces_count(void)
{
	pieces_count = starting_pieces_info.size();
}


void SlidingPuzzleSolver::set_ending_pieces(const json &starting_pieces_json)
{
	for (cell_id starting_piece_json_index = 0; starting_piece_json_index != pieces_count; ++starting_piece_json_index)
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


void SlidingPuzzleSolver::set_walls(const json &walls_json)
{
	for (const auto &wall_json : walls_json)
	{
		Wall wall;

		const json &wall_pos_json = wall_json["pos"];
		const json &wall_size_json = wall_json["size"];

		Pos &wall_pos = wall.pos;
		wall_pos.x = wall_pos_json["x"];
		wall_pos.y = wall_pos_json["y"];

		Size &wall_size = wall.size;
		wall_size.width = wall_size_json["width"];
		wall_size.height = wall_size_json["height"];

		walls.push_back(wall);
	}
}


void SlidingPuzzleSolver::set_width_and_height(void)
{
	// Keep in mind that this function assumes that walls will *always* surround the puzzle.

	width = 0;
	height = 0;

	for (const auto &wall : walls)
	{
		const auto &wall_pos = wall.pos;
		const auto &wall_size = wall.size;

		const auto x = wall_pos.x;
		const auto wall_width = wall_size.width;
		if (x + wall_width > width) width = x + wall_width;

		const auto y = wall_pos.y;
		const auto wall_height = wall_size.height;
		if (y + wall_height > height) height = y + wall_height;
	}
}


void SlidingPuzzleSolver::set_emptied_offsets(void)
{
	emptied_offsets.pieces.resize(pieces_count);

	// TODO: Turn the looping through all cells into an iterator/generator function.
	for (cell_id piece_index = 0; piece_index < pieces_count; ++piece_index)
	{
		const auto &starting_piece_info = starting_pieces_info[piece_index];

		const auto &piece_top_left = starting_piece_info.top_left;
		const auto &piece_top_left_x = piece_top_left.x;
		const auto &piece_top_left_y = piece_top_left.y;

		for (piece_direction direction = 0; direction < direction_count; ++direction)
		{
			for (const auto &rect : starting_piece_info.rects)
			{
				const auto &rect_offset = rect.offset;
				const auto &rect_offset_x = rect_offset.x;
				const auto &rect_offset_y = rect_offset.y;

				const auto &rect_size = rect.size;
				const auto &rect_width = rect_size.width;
				const auto &rect_height = rect_size.height;

				for (int rect_height_offset = 0; rect_height_offset < rect_height; ++rect_height_offset)
				{
					for (int rect_width_offset = 0; rect_width_offset < rect_width; ++rect_width_offset)
					{
						const auto cell_offset_x = rect_offset_x + rect_width_offset;
						const auto cell_offset_y = rect_offset_y + rect_height_offset;

						auto checked_x = piece_top_left_x + cell_offset_x;
						auto checked_y = piece_top_left_y + cell_offset_y;

						// See the documentation for emptied_offsets in the class header to understand why the switch looks like this.
						switch (direction)
						{
							case 0:
								checked_y++;
								break;
							case 1:
								checked_y--;
								break;
							case 2:
								checked_x++;
								break;
							case 3:
								checked_x--;
								break;
						}

						if (is_out_of_bounds(checked_x, checked_y))
						{
							add_offset_to_emptied_offsets(cell_offset_x, cell_offset_y, piece_index, direction);
							continue;
						}

						const auto &checked_index = cells[checked_y][checked_x];

						if (checked_index != piece_index)
						{
							add_offset_to_emptied_offsets(cell_offset_x, cell_offset_y, piece_index, direction);
						}
					}
				}
			}
		}
	}
}


bool SlidingPuzzleSolver::is_out_of_bounds(int x, int y)
{
	return (
		x < 0 || x >= width ||
		y < 0 || y >= height
	);
}


void SlidingPuzzleSolver::add_offset_to_emptied_offsets(const int x, const int y, const cell_id piece_index, const piece_direction direction)
{
	emptied_offsets.pieces[piece_index].directions[direction].offsets.push_back({
		.x = x,
		.y = y
	});
}


void SlidingPuzzleSolver::set_collision_offsets(void)
{
	collision_offsets.pieces.resize(pieces_count);

	for (cell_id piece_index = 0; piece_index < pieces_count; ++piece_index)
	{
		const auto &emptied_piece = emptied_offsets.pieces[piece_index];

		for (piece_direction collision_direction = 0; collision_direction < direction_count; ++collision_direction)
		{
			const auto &emptied_piece_direction = emptied_piece.directions[get_inverted_direction(collision_direction)];

			for (size_t offset_index = 0; offset_index < emptied_piece_direction.offsets.size(); ++offset_index)
			{
				const auto &emptied_offset = emptied_piece_direction.offsets[offset_index];
				
				auto collision_offset_x = emptied_offset.x;
				auto collision_offset_y = emptied_offset.y;

				// See the documentation for collision_offsets in the class header to understand why the switch looks like this.
				switch (collision_direction)
				{
					case 0:
						collision_offset_y--;
						break;
					case 1:
						collision_offset_y++;
						break;
					case 2:
						collision_offset_x--;
						break;
					case 3:
						collision_offset_x++;
						break;
				}

				collision_offsets.pieces[piece_index].directions[collision_direction].offsets.push_back({
					.x = collision_offset_x,
					.y = collision_offset_y
				});
			}
		}
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


void SlidingPuzzleSolver::initialize_variable_fields(const json &puzzle_json)
{
	initialize_cells();
	add_wall_cells(puzzle_json["walls"]);
	add_piece_cells();

	initialize_pieces();
}


void SlidingPuzzleSolver::initialize_cells(void)
{
	// The program assumes that unspecified cells are empty cells by default
	// as the puzzle JSON file doesn't specify where the empty cells are.
	cells = std::vector<std::vector<cell_id>>(height, std::vector<cell_id>(width, empty_cell_id));
}


void SlidingPuzzleSolver::add_wall_cells(const json &walls_json)
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


void SlidingPuzzleSolver::add_piece_cells(void)
{
	for (cell_id starting_piece_info_index = 0; starting_piece_info_index != pieces_count; ++starting_piece_info_index)
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


void SlidingPuzzleSolver::initialize_pieces(void)
{
	for (const auto &starting_piece_info : starting_pieces_info)
	{
		Piece piece;

		piece.top_left = starting_piece_info.top_left;

		pieces.push_back(piece);
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
	std::cout << std::endl;
}


const std::vector<std::vector<char>> SlidingPuzzleSolver::get_board()
{
	std::vector<std::vector<char>> board = get_2d_vector();

	set_pieces_on_board(board);
	set_walls_on_board(board);

	return board;
}


const std::vector<std::vector<char>> SlidingPuzzleSolver::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(height, std::vector<char>(width, empty_character));
}


void SlidingPuzzleSolver::set_pieces_on_board(std::vector<std::vector<char>> &board)
{
	for (cell_id piece_index = 0; piece_index != pieces_count; ++piece_index)
	{
		const Piece &piece = pieces[piece_index];

		const Pos &top_left = piece.top_left;
		const int top_left_x = top_left.x;
		const int top_left_y = top_left.y;

		const StartingPieceInfo &starting_piece_info = starting_pieces_info[piece_index];
		const std::vector<Rect> &rects = starting_piece_info.rects;

		for (const auto &rect : rects)
		{
			const Offset &rect_offset = rect.offset;
			const int top_left_rect_x = top_left_x + rect_offset.x;
			const int top_left_rect_y = top_left_y + rect_offset.y;

			const Size &rect_size = rect.size;
			const int rect_width = rect_size.width;
			const int rect_height = rect_size.height;

			for (int y = top_left_rect_y; y < top_left_rect_y + rect_height; ++y)
			{
				for (int x = top_left_rect_x; x < top_left_rect_x + rect_width; ++x)
				{
					board[y][x] = get_piece_label(piece_index);
				}
			}
		}
	}
}


char SlidingPuzzleSolver::get_piece_label(cell_id piece_index)
{
	return piece_labels[piece_index % piece_labels_length];
}


void SlidingPuzzleSolver::set_walls_on_board(std::vector<std::vector<char>> &board)
{
	for (const auto &wall : walls)
	{
		const Pos &wall_pos = wall.pos;
		const int top_left_wall_x = wall_pos.x;
		const int top_left_wall_y = wall_pos.y;

		const Size &wall_size = wall.size;

		for (int y = top_left_wall_y; y < top_left_wall_y + wall_size.height; ++y)
		{
			for (int x = top_left_wall_x; x < top_left_wall_x + wall_size.width; ++x)
			{
				board[y][x] = wall_character;
			}
		}
	}
}


bool SlidingPuzzleSolver::add_current_state(void)
{
	const std::pair<std::unordered_set<std::vector<Piece>, Piece::Hasher>::iterator, bool> insert_info = states.insert(pieces);

	const bool success = insert_info.second;

	return success;
}


void SlidingPuzzleSolver::solve(void)
{
	std::vector<Move> move_stack;

	int max_depth = 1;

	// TODO: Can this line be shortened?
	// TODO: Why can the std::ref() around move_stack be omitted, while this is not the case for max_depth?
	// std::thread timed_print_thread(&SlidingPuzzleSolver::timed_print, this, std::ref(move_stack), std::ref(max_depth));

	while (!finished)
	{
		// std::cout << std::endl << "----------------" << std::endl;

		states.clear();

		state_count = 0;
		// prev_state_count = 0;

		solve_up_till_max_depth(move_stack, max_depth);

		max_depth++;
	}

	// timed_print_thread.join();
}


void SlidingPuzzleSolver::solve_up_till_max_depth(std::vector<Move> &move_stack, int max_depth)
{
	Move start_move = {
		.next = { .index = 0, .direction = 0 },
		.undo = { .index = 0, .direction = no_undo }
	};

	move_stack.push_back(start_move);

	add_current_state();

	while (!move_stack.empty())
	{
		// print_board();

		update_finished();
		if (finished)
		{
			return;
		}

		Move &move = move_stack.back();

		int depth = move_stack.size();
		if ((depth >= max_depth) ||
			(no_next_piece_or_direction(move.next)) ||
			(move_piece(move.next.index, move.next.direction, move_stack) == false))
		{
			undo_move(move.undo);
			move_stack.pop_back();
		}
	}
}


bool SlidingPuzzleSolver::no_next_piece_or_direction(const MoveInfo &next)
{
	return (
		next.index == pieces_count - 1 &&
		next.direction == 3
	);
}


void SlidingPuzzleSolver::timed_print(const std::vector<Move> &move_stack, const int max_depth)
{
	std::cout << std::endl;

	while (!finished)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		timed_print_core(max_depth);
	}

	std::cout << std::endl << std::endl << "Path:" << std::endl;
	std::cout << "Often duplicate states seen:" << often_duplicate_states_seen << std::endl;
	std::cout << get_path_string(move_stack) << std::endl << std::endl;
}


void SlidingPuzzleSolver::timed_print_core(const int max_depth)
{
	// TODO: Store elapsed_time in something more appropriate than int.
	const int elapsed_time = get_elapsed_seconds().count();

	// const int states_count_diff = state_count - prev_state_count;
	// prev_state_count = state_count;

	std::cout << "\33[2K\r"; // Clears the line and goes back to the left.

	std::cout << "Elapsed time: " << elapsed_time << " seconds";

	KiloFormatter kf;

	std::cout << ", Max depth: " << kf.format(max_depth);
	std::cout << ", Unique states: " << kf.format(state_count);
	// std::cout << " (+" << kf.format(states_count_diff) << "/s)";

	std::cout << std::flush;
}


std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - start_time;
}


const std::string SlidingPuzzleSolver::get_path_string(const std::vector<Move> &move_stack)
{
	// If this method ever needs to be called a lot then try using a rope instead.
	std::stringstream path_stringstream;

	auto reversed_move_stack = get_reversed_move_stack(move_stack);

	while (!reversed_move_stack.empty())
	{
		const auto &move = reversed_move_stack.back();
		
		if (move.undo.direction != no_undo)
		{
			const cell_id piece_index = move.undo.index;
			path_stringstream << get_piece_label(piece_index);

			const piece_direction direction = get_inverted_direction(move.undo.direction);
			path_stringstream << direction_characters[direction];
		}

		reversed_move_stack.pop_back();
	}

	return path_stringstream.str();
}


std::vector<Move> SlidingPuzzleSolver::get_reversed_move_stack(std::vector<Move> move_stack)
{
	std::vector<Move> reversed_move_stack;

	while (!move_stack.empty())
	{
		const auto &move = move_stack.back();
		
		reversed_move_stack.push_back(move);
		
		move_stack.pop_back();
	}

	return reversed_move_stack;
}


void SlidingPuzzleSolver::update_finished()
{
	finished = true;

	for (const auto &ending_piece : ending_pieces)
	{
		const cell_id ending_piece_index = ending_piece.piece_index;
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


bool SlidingPuzzleSolver::move_piece(cell_id &start_piece_index, piece_direction &start_direction, std::vector<Move> &move_stack)
{
	for (cell_id &piece_index = start_piece_index; piece_index != pieces_count; ++piece_index)
	{
		Piece &piece = pieces[piece_index];
		Pos &piece_top_left = piece.top_left;

		for (piece_direction &direction = start_direction; direction < direction_count; ++direction)
		{
			if (cant_move(piece_top_left, piece_index, direction))
			{
				continue;
			}

			move(piece_top_left, piece_index, direction);

			if (add_current_state())
			{
				move_stack.push_back({
					.next = { .index = 0, .direction = 0 },
					.undo = { .index = piece_index, .direction = get_inverted_direction(direction) }
				});

				start_piece_index = get_next_piece_index(piece_index, direction);
				start_direction = get_next_direction(direction);

				state_count++;

				often_duplicate_states_seen++;

				return true;
			}

			move(piece_top_left, piece_index, get_inverted_direction(direction));
		}

		start_direction = 0; // TODO: Find better approach.
	}

	return false;
}


bool SlidingPuzzleSolver::cant_move(const Pos &piece_top_left, cell_id piece_index, piece_direction direction)
{
	const auto &collision_piece = collision_offsets.pieces[piece_index];

	const auto &collision_piece_direction = collision_piece.directions[direction];

	for (const auto offset : collision_piece_direction.offsets)
	{
		const auto x = piece_top_left.x + offset.x;
		const auto y = piece_top_left.y + offset.y;

		if (is_out_of_bounds(x, y))
		{
			return true;
		}

		if (cells[y][x] != empty_cell_id)
		{
			return true;
		}
	}

	return false;
}


void SlidingPuzzleSolver::move(Pos &piece_top_left, const cell_id piece_index, const piece_direction direction)
{
	const auto &piece_emptied_offsets = emptied_offsets.pieces[piece_index].directions[direction].offsets;

	apply_offsets_to_cells(piece_top_left, piece_emptied_offsets, empty_cell_id);

	const auto &piece_collision_offsets = collision_offsets.pieces[piece_index].directions[direction].offsets;

	apply_offsets_to_cells(piece_top_left, piece_collision_offsets, piece_index);

	move_piece_top_left(piece_top_left, direction);
}


void SlidingPuzzleSolver::apply_offsets_to_cells(Pos &piece_top_left, const std::vector<Offset> &offsets, const cell_id index)
{
	for (const auto &offset : offsets)
	{
		const int x = piece_top_left.x + offset.x;
		const int y = piece_top_left.y + offset.y;

		cells[y][x] = index;
	}
}


void SlidingPuzzleSolver::move_piece_top_left(Pos &piece_top_left, const piece_direction direction)
{
	switch (direction)
	{
	case 0:
		piece_top_left.y--;
		break;
	case 1:
		piece_top_left.y++;
		break;
	case 2:
		piece_top_left.x--;
		break;
	case 3:
		piece_top_left.x++;
		break;
	}
}


cell_id SlidingPuzzleSolver::get_next_piece_index(const cell_id &piece_index, const piece_direction &direction)
{
	// Keeps the same piece by going to the next direction.
	if (direction <= 2) return piece_index;

	// If this is reached then direction == 3, so go to the next piece.
	return piece_index + 1;
}


piece_direction SlidingPuzzleSolver::get_next_direction(const piece_direction &direction)
{
	return (direction + 1) % direction_count;
}


void SlidingPuzzleSolver::undo_move(const MoveInfo &undo)
{
	if (undo.direction != no_undo)
	{
		Piece &undo_piece = pieces[undo.index];

		move(undo_piece.top_left, undo.index, undo.direction);
	}
}


// bool SlidingPuzzleSolver::a_rect_cant_be_moved(const std::vector<Rect> &rects, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left)
// {
// 	for (const auto &rect : rects)
// 	{
// 		if (cant_move(rect, direction, piece_id, piece_top_left))
// 		{
// 			return true;
// 		}
// 	}

// 	return false;
// }


// bool SlidingPuzzleSolver::cant_move(const Rect &rect, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left)
// {
// 	/*
// 	Only the cells on the edges are checked for a collision, in this numbered order:
// 	11111
// 	3   4
// 	3   4
// 	22222
// 	*/

// 	const int rect_left_x = piece_top_left.x + rect.offset.x;
// 	const int rect_top_y = piece_top_left.y + rect.offset.y;

// 	const Size &rect_size = rect.size;

// 	int start_x = rect_left_x;
// 	int start_y = rect_top_y;

// 	switch (direction)
// 	{
// 	case 0:
// 		start_y--;
// 		break;
// 	case 1:
// 		start_y++;
// 		break;
// 	case 2:
// 		start_x--;
// 		break;
// 	case 3:
// 		start_x++;
// 		break;
// 	}

// 	return cant_move_in_direction(piece_id, start_x, start_y, rect_size);
// }


// bool SlidingPuzzleSolver::cant_move_in_direction(const cell_id piece_id, const int start_x, const int start_y, const Size &rect_size)
// {
// 	const int rect_height = rect_size.height;
// 	const int rect_width = rect_size.width;

// 	for (int y_offset = 0; y_offset < rect_height; ++y_offset)
// 	{
// 		for (int x_offset = 0; x_offset < rect_width; ++x_offset)
// 		{
// 			if (cant_move_to_cell(piece_id, start_x + x_offset, start_y + y_offset))
// 			{
// 				return true;
// 			}
// 		}
// 	}

// 	return false;
// }


// bool SlidingPuzzleSolver::cant_move_to_cell(const cell_id piece_id, const int x, const int y)
// {
// 	if (x < 0 || x >= width ||
// 		y < 0 || y >= height)
// 	{
// 		return true;
// 	}

// 	const cell_id checked_cell_id = cells[y][x];

// 	if (checked_cell_id != empty_cell_id && checked_cell_id != piece_id)
// 	{
// 		return true;
// 	}

// 	return false;
// }


// bool SlidingPuzzleSolver::move_doesnt_cross_puzzle_edge(const cell_id piece_index, const Pos &piece_top_left)
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


// bool SlidingPuzzleSolver::no_intersection(const cell_id piece_index_1, const Pos &piece_1_pos)
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

// 	for (cell_id piece_index_2 = 0; piece_index_2 != pieces_count; ++piece_index_2)
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
