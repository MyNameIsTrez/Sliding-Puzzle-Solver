#include "sliding_puzzle_solver.hpp"

SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	const json puzzle_json = get_puzzle_json(exe_path, puzzle_name);

	set_constant_fields(puzzle_json);
	initialize_variable_fields(puzzle_json);

	set_emptied_cell_offsets();
	set_collision_checked_offsets();
}

void SlidingPuzzleSolver::run(void)
{
	print_board();

	add_current_state();

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

void SlidingPuzzleSolver::set_emptied_cell_offsets(void)
{
	emptied_cell_offsets.pieces.resize(pieces_count);

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

						// See the documentation for emptied_cell_offsets in the class header to understand why the switch looks like this.
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

						if (checked_x < 0 || checked_x >= width ||
							checked_y < 0 || checked_y >= height)
						{
							add_offset_to_emptied_cell_offsets(cell_offset_x, cell_offset_y, piece_index, direction);
							continue;
						}

						const auto &checked_index = cells[checked_y][checked_x];

						if (checked_index != piece_index)
						{
							add_offset_to_emptied_cell_offsets(cell_offset_x, cell_offset_y, piece_index, direction);
						}
					}
				}
			}
		}
	}
}

void SlidingPuzzleSolver::add_offset_to_emptied_cell_offsets(const int x, const int y, const cell_id piece_index, const piece_direction direction)
{
	// TODO: Initialize offset as a const somehow.
	Offset offset;
	offset.x = x;
	offset.y = y;

	emptied_cell_offsets.pieces[piece_index].directions[direction].offsets.push_back(offset);
}

void SlidingPuzzleSolver::set_collision_checked_offsets(void)
{
	collision_checked_offsets.pieces.resize(pieces_count);

	for (cell_id piece_index = 0; piece_index < pieces_count; ++piece_index)
	{
		const auto &piece = emptied_cell_offsets.pieces[piece_index];

		for (piece_direction direction = 0; direction < direction_count; ++direction)
		{
			const auto &piece_direction = piece.directions[get_inverted_direction(direction)];

			for (size_t offset_index = 0; offset_index < piece_direction.offsets.size(); ++offset_index)
			{
				const auto &offset = piece_direction.offsets[offset_index];
				auto cell_offset_x = offset.x;
				auto cell_offset_y = offset.y;

				// See the documentation for collision_checked_offsets in the class header to understand why the switch looks like this.
				switch (direction)
				{
					case 0:
						cell_offset_y--;
						break;
					case 1:
						cell_offset_y++;
						break;
					case 2:
						cell_offset_x--;
						break;
					case 3:
						cell_offset_x++;
						break;
				}

				add_offset_to_collision_checked_offsets(cell_offset_x, cell_offset_y, piece_index, direction);
			}
		}
	}
}

// TODO: Somehow combine this method with add_offset_to_emptied_cell_offsets()
// without having to constantly pass emptied_cell_offsets/collision_checked_offsets as an argument.
void SlidingPuzzleSolver::add_offset_to_collision_checked_offsets(const int x, const int y, const cell_id piece_index, const piece_direction direction)
{
	// TODO: Initialize offset as a const somehow.
	Offset offset;
	offset.x = x;
	offset.y = y;

	collision_checked_offsets.pieces[piece_index].directions[direction].offsets.push_back(offset);
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
					board[y][x] = piece_labels[piece_index % piece_labels_length];
				}
			}
		}
	}
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
	std::stack<Move> move_stack;

	Move start_move = {
		.next = { .index = 0, .direction = 0 },
		.undo = { .index = 0, .direction = 0 }
	};

	move_stack.push(start_move);

	// TODO: Can this line be shortened?
	// std::thread timed_print_thread(&SlidingPuzzleSolver::timed_print, this, std::ref(path_stack), std::ref(move_stack));

	while (!move_stack.empty())
	{
		update_finished();
		if (finished)
		{
			break;
		}

		Move &move = move_stack.top();

		if (no_next_piece_or_direction(move.next))
		{
			continue;
		}

		if (move_piece(start_piece_index, start_direction, move_stack) == false)
		{
			// If no piece could be moved, recurse back.
			if (move.undo.direction != no_undo)
			{
				recover_piece(move.undo);
			}

			move_stack.pop();
		}
	}

	// timed_print_thread.join();
}

bool SlidingPuzzleSolver::no_next_piece_or_direction(const MoveInfo &next)
{
	return (
		next.index == pieces_count - 1 &&
		next.direction == 3
	);
}

cell_id SlidingPuzzleSolver::get_next_piece_index(const cell_id &piece_index, const piece_direction &direction)
{
	// Keeps the same piece by going to the next direction.
	if (direction < 3) return piece_index;

	// If this is reached then direction == 3, so go to the next piece.
	return piece_index + 1;
}

piece_direction SlidingPuzzleSolver::get_next_direction(const piece_direction &direction)
{
	return (direction + 1) % direction_count;
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

void SlidingPuzzleSolver::recover_piece(const MoveInfo &undo)
{
	Piece &undo_piece = pieces[undo.index];

	move(undo_piece.top_left, undo.index, undo.direction);
}

void SlidingPuzzleSolver::move(Pos &piece_top_left, const cell_id piece_index, const piece_direction direction)
{
	// set_piece_cell_ids(piece_top_left, empty_cell_id);

	move_piece_top_left(piece_top_left, direction);

	// set_piece_cell_ids(piece_top_left, piece_index);
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

// void SlidingPuzzleSolver::set_piece_cell_ids(const Rect &rect, const int start_x, const int start_y, const cell_id &id)
// {
// 	const Size &rect_size = rect.size;
// 	const int rect_height = rect_size.height;
// 	const int rect_width = rect_size.width;

// 	for (int y_offset = 0; y_offset < rect_height; ++y_offset)
// 	{
// 		for (int x_offset = 0; x_offset < rect_width; ++x_offset)
// 		{
// 			cells[start_y + y_offset][start_x + x_offset] = id;
// 		}
// 	}
// }

// void SlidingPuzzleSolver::timed_print(const std::stack<std::vector<std::pair<cell_id, char>>> &path_stack, const std::stack<std::vector<Piece>> &move_stack)
// {
// 	std::cout << std::endl;

// 	while (!finished)
// 	{
// 		std::this_thread::sleep_for(std::chrono::seconds(1));
// 		timed_print_core(path_stack, move_stack);
// 	}

// 	std::cout << std::endl << std::endl << "Path:" << std::endl;
// 	std::cout << get_path_string(path_stack.top()) << std::endl << std::endl;
// }

// void SlidingPuzzleSolver::timed_print_core(const std::stack<std::vector<std::pair<cell_id, char>>> &path_stack, const std::stack<std::vector<Piece>> &move_stack)
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
// 	std::cout << ", Queue length: " << kf.format(move_stack.size());
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

bool SlidingPuzzleSolver::move_piece(cell_id &start_piece_index, piece_direction &start_direction, std::stack<Move> &move_stack)
{
	for (cell_id &piece_index = start_piece_index; piece_index != pieces_count; ++piece_index)
	{
		Piece &piece = pieces[piece_index];
		Pos &piece_top_left = piece.top_left;

		const StartingPieceInfo &starting_piece_info = starting_pieces_info[piece_index];
		const std::vector<Rect> &rects = starting_piece_info.rects;

		const int piece_top_left_x_backup = piece_top_left.x;
		const int piece_top_left_y_backup = piece_top_left.y;

		for (piece_direction &direction = start_direction; direction < direction_count; ++direction)
		{
			if (a_rect_cant_be_moved(rects, direction, piece_index, piece_top_left))
			{
				continue;
			}

			move(piece_top_left, piece_index, direction);

			if (add_current_state())
			{
				move_stack.push({
					.next = { .index = 0, .direction = 0 },
					.undo = { .index = piece_index, .direction = get_inverted_direction(direction) }
				});

				start_piece_index = get_next_piece_index(piece_index, direction);
				start_direction = get_next_direction(direction);

				state_count++;

				return true;
			}

			piece_top_left.x = piece_top_left_x_backup;
			piece_top_left.y = piece_top_left_y_backup;
		}

		start_direction = 0; // TODO: Find better approach.
	}

	return false;
}

bool SlidingPuzzleSolver::a_rect_cant_be_moved(const std::vector<Rect> &rects, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left)
{
	for (const auto &rect : rects)
	{
		if (cant_move(rect, direction, piece_id, piece_top_left))
		{
			return true;
		}
	}

	return false;
}

bool SlidingPuzzleSolver::cant_move(const Rect &rect, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left)
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

	int start_x = rect_left_x;
	int start_y = rect_top_y;

	switch (direction)
	{
	case 0:
		start_y--;
		break;
	case 1:
		start_y++;
		break;
	case 2:
		start_x--;
		break;
	case 3:
		start_x++;
		break;
	}

	return cant_move_in_direction(piece_id, start_x, start_y, rect_size);
}

bool SlidingPuzzleSolver::cant_move_in_direction(const cell_id piece_id, const int start_x, const int start_y, const Size &rect_size)
{
	const int rect_height = rect_size.height;
	const int rect_width = rect_size.width;

	for (int y_offset = 0; y_offset < rect_height; ++y_offset)
	{
		for (int x_offset = 0; x_offset < rect_width; ++x_offset)
		{
			if (cant_move_to_cell(piece_id, start_x + x_offset, start_y + y_offset))
			{
				return true;
			}
		}
	}

	return false;
}

bool SlidingPuzzleSolver::cant_move_to_cell(const cell_id piece_id, const int x, const int y)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height)
	{
		return true;
	}

	const cell_id checked_cell_id = cells[y][x];

	if (checked_cell_id != empty_cell_id && checked_cell_id != piece_id)
	{
		return true;
	}

	return false;
}

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

// const std::string SlidingPuzzleSolver::get_path_string(const std::vector<std::pair<cell_id, char>> &path)
// {
// 	// If this method ever needs to be called a lot then try using a rope instead.
// 	std::stringstream path_stringstream;

// 	for (std::vector<std::pair<cell_id, char>>::const_iterator pair_it = path.cbegin(); pair_it != path.cend(); ++pair_it)
// 	{
// 		const cell_id piece_index = pair_it->first;
// 		path_stringstream << piece_index;

// 		const char direction = pair_it->second;
// 		path_stringstream << direction;
// 	}

// 	return path_stringstream.str();
// }
