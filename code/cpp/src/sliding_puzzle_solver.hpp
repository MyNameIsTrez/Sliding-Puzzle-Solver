#pragma once


#include <vector>
#include <unordered_set>
#include <queue>

#include <iostream>
#include <fstream>
#include <chrono>

#include <thread>
#include <filesystem>


typedef int cell_id;
typedef int piece_direction;


typedef std::vector<std::vector<cell_id>> cells_t;

struct Piece;
typedef std::vector<Piece> pieces_t;

typedef std::queue<std::pair<pieces_t, cells_t>> pieces_queue_t;


#include "json.hpp"
using json = nlohmann::json;

#include "pieces.hpp"
#include "kilo_formatter.h"


class SlidingPuzzleSolver
{
public:
	SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name);
	void solve(void);

private:
	// Static consts, structs and enums ////////
	static int const direction_count = 4;

	int const no_undo = -1;

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
	pieces_directions_cell_offsets emptied_offsets;

	/*
	If this piece needs to check if it can move left:
	" pppp"
	" p  p"

	then these "#" characters denote the cells it will check for collision:
	"#pppp"
	"#p #p"

	These "#" characters are stored here as offsets relative to the top-left "p".
	*/
	pieces_directions_cell_offsets collision_offsets;

	int pieces_count;

	cells_t starting_cells;


	// Variables ////////
	std::unordered_set<pieces_t, Piece::HashFunction> states;

	int state_count = 0;
	int prev_state_count = 0;

	bool finished = false;


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

	void set_emptied_offsets(void);
	bool is_out_of_bounds(int x, int y);
	void add_offset_to_emptied_offsets(const int x, const int y, const cell_id piece_index, const piece_direction direction);

	void set_collision_offsets(void);
	piece_direction get_inverted_direction(const piece_direction &direction);

	// Initialize variables
	void initialize_variable_fields(const json &puzzle_json);

	void set_starting_cells(void);
	void add_wall_cells(const json &walls_json);
	void add_piece_cells(void);



	pieces_t get_starting_pieces(void);



	// Print board
	void print_board(const pieces_t &pieces);
	const std::vector<std::vector<char>> get_board(const pieces_t &pieces);
	const std::vector<std::vector<char>> get_2d_vector(void);
	void set_pieces_on_board(std::vector<std::vector<char>> &board, const pieces_t &pieces);
	char get_piece_label(cell_id piece_index);
	void set_walls_on_board(std::vector<std::vector<char>> &board);

	bool add_state(const pieces_t &pieces);

	void update_finished(const pieces_t &pieces);

	// Move Pieces
	void queue_valid_moves(pieces_queue_t &pieces_queue, pieces_t &pieces, cells_t &cells);
	bool cant_move(const Pos &piece_top_left, const cell_id piece_index, const piece_direction direction, cells_t &cells);
	void move(Pos &piece_top_left, const cell_id piece_index, const piece_direction direction, cells_t &cells);
	void apply_offsets_to_cells(cells_t &cells, Pos &piece_top_left, const std::vector<Offset> &offsets, const cell_id index);
	void move_piece_top_left(Pos &piece_top_left, const piece_direction direction);

	pieces_t get_pieces_copy(const pieces_t &pieces);
	cells_t get_cells_copy(const cells_t &cells);

	// Print progress
	void timed_print(const pieces_queue_t &pieces_queue);
	void timed_print_core(void);
	std::chrono::duration<double> get_elapsed_seconds(void);

	// const std::string get_path_string(const std::vector<Move> &move_stack);
	// std::vector<Move> get_reversed_move_stack(std::vector<Move> move_stack);

	// bool a_rect_cant_be_moved(const std::vector<Rect> &rects, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left);
	// bool cant_move(const Rect &rect, const piece_direction &direction, const cell_id piece_id, const Pos &piece_top_left);
	// bool cant_move_in_direction(const cell_id piece_id, const int start_x, const int start_y, const Size &rect_size);
	// bool cant_move_to_cell(const cell_id piece_id, const int x, const int y);

	// bool move_doesnt_cross_puzzle_edge(const std::size_t piece_index, const Pos &piece_pos);
	// bool no_intersection(const std::size_t piece_index_1, const Pos &piece_1_pos);
};
