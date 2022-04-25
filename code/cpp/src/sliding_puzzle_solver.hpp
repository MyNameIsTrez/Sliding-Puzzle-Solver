#pragma once

////////

#include "json.hpp"
using json = nlohmann::json;

////////

#include "pieces.hpp"
#include "kilo_formatter.h"

////////

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <unordered_set>
#include <stack>
#include <thread>
#include <filesystem>

////////

typedef int cell_id;
typedef int piece_direction;

////////

class SlidingPuzzleSolver
{
	// Static consts, structs and enums ////////
	static int const direction_count = 4;

	int const no_recovery = -1;

	struct pieces_directions_cell_offsets
	{
		struct directions
		{
			struct offsets
			{
				std::vector<Offset> offsets;
			};
			std::array<offsets, direction_count> directions;
		};
		std::vector<directions> pieces;
	};

	enum
	{
		empty_cell_id = -1,
		wall_cell_id = -2
	};

	// Constants ////////
	const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

	const char empty_character = ' ';
	const char wall_character = '#';

	const std::vector<char> direction_characters = {'^', 'v', '<', '>'};

	// TODO: Support more than 26 piece labels in some way.
	const std::string piece_labels = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::size_t piece_labels_length = piece_labels.length();

	const bool print_board_every_path = false;


	// Constants after constructor ////////
	std::vector<Wall> walls;

	int width;
	int height;

	std::vector<StartingPieceInfo> starting_pieces_info;
	std::vector<EndingPiece> ending_pieces;

	/*
	If this piece needs to move left:
	" pppp"
	" p  p"

	then these "#" characters denote the cells it will turn into empty cells:
	"pppp#"
	"p# p#"

	These "#" characters are stored here as offsets relative to the top-left "p".
	*/
	pieces_directions_cell_offsets emptied_cell_offsets;

	/*
	If this piece needs to check if it can move left:
	" pppp"
	" p  p"

	then these "#" characters denote the cells it will check for collision:
	"#pppp"
	"#p #p"

	These "#" characters are stored here as offsets relative to the top-left "p".
	*/
	pieces_directions_cell_offsets collision_checked_offsets;

	int pieces_count;


	// Variables ////////
	std::unordered_set<std::vector<Piece>, Piece::Hasher> states;

	int state_count = 0;
	int prev_state_count = 0;
	bool finished = false;

	std::vector<std::vector<cell_id>> cells;

	std::vector<Piece> pieces;


	// Methods ////////
	const json get_puzzle_json(std::filesystem::path &exe_path, const std::string &puzzle_name);
	const std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path &exe_path, const std::string &puzzle_name);

	// Set constants
	void set_constant_fields(const json &puzzle_json);

	void set_starting_pieces_info(const json &starting_pieces_info_json);
	void set_pieces_count(void);

	void set_ending_pieces(const json &starting_pieces_json);

	void set_walls(const json &walls_json);
	void set_width_and_height(void);

	void set_emptied_cell_offsets(void);
	void add_offset_to_emptied_cell_offsets(const int x, const int y, const cell_id piece_index, const piece_direction direction);

	void set_collision_checked_offsets(void);
	void add_offset_to_collision_checked_offsets(const int x, const int y, const cell_id piece_index, const piece_direction direction);

	// Initialize variables
	void initialize_variable_fields(const json &puzzle_json);

	void initialize_cells(void);
	void add_wall_cells(const json &walls_json);
	void add_piece_cells(void);

	void initialize_pieces(void);

	// Print board
	void print_board(void);
	const std::vector<std::vector<char>> get_board(void);
	const std::vector<std::vector<char>> get_2d_vector(void);
	void set_pieces_on_board(std::vector<std::vector<char>> &board);
	void set_walls_on_board(std::vector<std::vector<char>> &board);

	bool add_current_state(void);

	void solve(void);
	bool no_next_piece_or_direction(const cell_id &start_piece_index, const piece_direction &start_direction);
	void recover_piece(const cell_id &recovery_piece_index, const piece_direction &recovery_direction);
	void move(Pos &piece_top_left, const piece_direction direction, const std::vector<Rect> &rects, const cell_id &piece_id);
	void move_piece_top_left(Pos &piece_top_left, const piece_direction direction);
	void set_piece_cell_ids(const Rect &rect, const int start_x, const int start_y, const cell_id &id);

	// Print progress
	// void timed_print(const std::stack<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::stack<std::vector<Piece>> &pieces_queue);
	// void timed_print_core(const std::stack<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::stack<std::vector<Piece>> &pieces_queue);
	// std::chrono::duration<double> get_elapsed_seconds(void);

	void update_finished(void);

	// Move a Piece
	bool move_piece(cell_id &start_piece_index, piece_direction &start_direction, std::stack<std::tuple<cell_id, piece_direction, cell_id, piece_direction>> &pieces_stack);
	bool a_rect_cant_be_moved(const std::vector<Rect> &rects, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left);
	bool cant_move(const Rect &rect, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left);
	bool cant_move_in_direction(const cell_id piece_id, const int start_x, const int start_y, const Size &rect_size);
	bool cant_move_to_cell(const cell_id piece_id, const int x, const int y);

	piece_direction get_inverted_direction(const piece_direction &direction);
	cell_id get_next_piece_index(const cell_id &piece_index, const piece_direction &direction);
	piece_direction get_next_direction(const piece_direction &direction);

	bool move_doesnt_cross_puzzle_edge(const std::size_t piece_index, const Pos &piece_pos);
	bool no_intersection(const std::size_t piece_index_1, const Pos &piece_1_pos);
	const std::vector<Piece> deepcopy_pieces_positions(void);

	const std::string get_path_string(const std::vector<std::pair<std::size_t, char>> &path);

public:
	SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name);
	void run(void);
};
