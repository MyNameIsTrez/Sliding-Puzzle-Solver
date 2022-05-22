#include "sliding_puzzle_solver.hpp"


SlidingPuzzleSolver::SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name)
{
	const json puzzle_json = get_puzzle_json(exe_path, puzzle_name);

	set_constant_fields(puzzle_json);
	initialize_variable_fields(puzzle_json);

	set_emptied_offsets();
	set_collision_offsets();
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

	set_starting_cells();
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


void SlidingPuzzleSolver::set_starting_cells(void)
{
	// The program assumes that unspecified cells are empty cells by default
	// as the puzzle JSON file doesn't specify where the empty cells are.
	starting_cells = cells_t(height, std::vector<cell_id>(width, empty_cell_id));
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

						const auto &checked_index = starting_cells[checked_y][checked_x];

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
	add_wall_cells(puzzle_json["walls"]);
	add_piece_cells();
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
				starting_cells[wall.pos.y + y_offset][wall.pos.x + x_offset] = wall_cell_id;
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
					starting_cells[rect_top_left_y + y_offset][rect_top_left_x + x_offset] = starting_piece_info_index;
				}
			}
		}
	}
}


pieces_t SlidingPuzzleSolver::get_starting_pieces(void)
{
	pieces_t starting_pieces;

	for (const auto &starting_piece_info : starting_pieces_info)
	{
		Piece piece;

		piece.top_left = starting_piece_info.top_left;

		starting_pieces.push_back(piece);
	}

	return starting_pieces;
}


void SlidingPuzzleSolver::print_board(const pieces_t &pieces)
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


const std::vector<std::vector<char>> SlidingPuzzleSolver::get_board(const pieces_t &pieces)
{
	std::vector<std::vector<char>> board = get_2d_vector();

	set_pieces_on_board(board, pieces);
	set_walls_on_board(board);

	return board;
}


const std::vector<std::vector<char>> SlidingPuzzleSolver::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(height, std::vector<char>(width, empty_character));
}


void SlidingPuzzleSolver::set_pieces_on_board(std::vector<std::vector<char>> &board, const pieces_t &pieces)
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


bool SlidingPuzzleSolver::add_state(const pieces_t &pieces)
{
	const std::pair<std::unordered_set<pieces_t, Piece::HashFunction>::iterator, bool> insert_info = states.insert(pieces);

	const bool success = insert_info.second;

	return success;
}


void SlidingPuzzleSolver::solve(void)
{
	const auto starting_pieces = get_starting_pieces();

	print_board(starting_pieces);

	add_state(starting_pieces);

	pieces_queue_t pieces_queue;

	pieces_queue.push({starting_pieces, starting_cells});

	path_queue_t path_queue;
	const path_t initial_empty_path = std::vector<std::pair<cell_id, piece_direction>>();
	path_queue.push(initial_empty_path);

	// TODO: Can this line be shortened?
	std::thread timed_print_thread(&SlidingPuzzleSolver::timed_print, this, std::ref(pieces_queue), std::ref(path_queue));

	while (!pieces_queue.empty())
	{
		auto [pieces, cells] = pieces_queue.front();
		pieces_queue.pop();

		// print_board(pieces);
		// std::cout << std::endl;

		update_finished(pieces);
		if (finished)
		{
			break;
		}

		const path_t path = path_queue.front();
		path_queue.pop(); // Purposely placed *after* the break above, as timed_print() is responsible for printing the final path.

		queue_valid_moves(pieces_queue, pieces, cells, path_queue, path);
	}

	timed_print_thread.join();
}


void SlidingPuzzleSolver::timed_print(const pieces_queue_t &pieces_queue, const path_queue_t &path_queue)
{
	std::cout << std::endl;

	while (!finished)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		timed_print_core(pieces_queue, path_queue);
	}

	std::cout << std::endl << std::endl << "Path:" << std::endl << get_path_string(path_queue.front()) << std::endl << std::endl;
}


void SlidingPuzzleSolver::timed_print_core(const pieces_queue_t &pieces_queue, const path_queue_t &path_queue)
{
	// TODO: Store elapsed_time in something more appropriate than int.
	const int elapsed_time = get_elapsed_seconds().count();

	const int states_count_diff = state_count - prev_state_count;
	prev_state_count = state_count;

	std::cout << "\33[2K\r"; // Clears the line and goes back to the left.

	std::cout << "Elapsed time: " << elapsed_time << " seconds";

	KiloFormatter kf;

	if (path_queue.size() > 0)
	{
		std::cout << ", Path length: " << kf.format(path_queue.front().size());
	}

	std::cout << ", Unique states: " << kf.format(state_count) << " (+" << kf.format(states_count_diff) << "/s)";

	std::cout << ", Queue length: " << kf.format(pieces_queue.size());

	std::cout << std::flush;
}


std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - start_time;
}


void SlidingPuzzleSolver::update_finished(const pieces_t &pieces)
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


void SlidingPuzzleSolver::queue_valid_moves(pieces_queue_t &pieces_queue, pieces_t &pieces, cells_t &cells, path_queue_t &path_queue, const path_t &path)
{
	for (cell_id piece_index = 0; piece_index != pieces_count; ++piece_index)
	{
		Piece &piece = pieces[piece_index];
		Pos &piece_top_left = piece.top_left;

		for (piece_direction direction = 0; direction < direction_count; ++direction)
		{
			if (cant_move(piece_top_left, piece_index, direction, cells))
			{
				continue;
			}

			move(piece_top_left, piece_index, direction, cells);

			if (add_state(pieces))
			{
				pieces_queue.push({get_pieces_copy(pieces), get_cells_copy(cells)});

				path_t new_path = get_path_copy(path);

				new_path.push_back({piece_index, direction});

				path_queue.push(new_path);

				state_count++;
			}

			move(piece_top_left, piece_index, get_inverted_direction(direction), cells);
		}
	}
}


bool SlidingPuzzleSolver::cant_move(const Pos &piece_top_left, const cell_id piece_index, const piece_direction direction, cells_t &cells)
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


void SlidingPuzzleSolver::move(Pos &piece_top_left, const cell_id piece_index, const piece_direction direction, cells_t &cells)
{
	const auto &piece_emptied_offsets = emptied_offsets.pieces[piece_index].directions[direction].offsets;

	apply_offsets_to_cells(cells, piece_top_left, piece_emptied_offsets, empty_cell_id);

	const auto &piece_collision_offsets = collision_offsets.pieces[piece_index].directions[direction].offsets;

	apply_offsets_to_cells(cells, piece_top_left, piece_collision_offsets, piece_index);

	move_piece_top_left(piece_top_left, direction);
}


void SlidingPuzzleSolver::apply_offsets_to_cells(cells_t &cells, Pos &piece_top_left, const std::vector<Offset> &offsets, const cell_id index)
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


pieces_t SlidingPuzzleSolver::get_pieces_copy(const pieces_t &pieces)
{
	const pieces_t pieces_copy = pieces;

	return pieces_copy;
}


cells_t SlidingPuzzleSolver::get_cells_copy(const cells_t &cells)
{
	const cells_t cells_copy = cells;

	return cells_copy;
}


path_t SlidingPuzzleSolver::get_path_copy(const path_t &path)
{
	const path_t path_copy = path;

	return path_copy;
}


std::string SlidingPuzzleSolver::get_path_string(const path_t &path)
{
	// If this method ever needs to be called a lot then try using a rope instead.
	std::stringstream path_stringstream;

	for (path_t::const_iterator pair_it = path.cbegin(); pair_it != path.cend(); ++pair_it)
	{
		std::size_t piece_index = pair_it->first;
		path_stringstream << piece_labels[piece_index];

		char direction = pair_it->second;
		path_stringstream << direction_characters[direction];
	}

	return path_stringstream.str();
}
