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

////////

typedef int cell_id;
typedef int piece_direction;

enum
{
	empty_cell_id = -1,
	wall_cell_id = -2
}

#define NO_RECOVERY -1

////////

class SlidingPuzzleSolver
{
	// Constants ////////
	int width;
	int height;

	std::vector<StartingPieceInfo> starting_pieces_info;
	std::vector<EndingPiece> ending_pieces;

	const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

	const char empty_character = ' ';
	const char wall_character = '#';

	const std::vector<char> direction_characters = {'^', 'v', '<', '>'};

	// TODO: Support more than 26 piece labels in some way.
	const std::string piece_labels = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::size_t piece_labels_length = piece_labels.length();

	const bool print_board_every_path = false;


	// Variables ////////
	// std::unordered_set<std::size_t> states;
	std::unordered_set<std::vector<Piece>, Piece::Hasher> states;

	int state_count = 0;
	int prev_state_count = 0;
	bool finished = false;

	// The program assumes that unspecified cells are empty cells by default
	// as the puzzle JSON file doesn't specify where the empty cells are.
	std::vector<std::vector<cell_id>> cells = std::vector<std::vector<cell_id>>(height, std::vector<cell_id>(width, empty_cell_id));

	// std::vector<std::vector<bool>> active_cells;

	std::vector<Piece> pieces;


	// Methods ////////
	const json get_puzzle_json(std::filesystem::path &exe_path, const std::string &puzzle_name);
	const std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path &exe_path, const std::string &puzzle_name);

	void initialize_constant_fields(const json &puzzle_json);

	void set_starting_pieces_info(const json &starting_pieces_info_json);
	void set_ending_pieces(const json &starting_pieces_json);

	void set_width_and_height(const json &walls_json);
	void set_wall_cells(const json &walls_json);
	void set_piece_cells(void);
	// int get_index(int x, int y);

	void print_board(void);
	const std::vector<std::vector<char>> get_board(void);
	const std::vector<std::vector<char>> get_2d_vector(void);
	void set_pieces_on_board(std::vector<std::vector<char>> &board);
	void set_walls_on_board(std::vector<std::vector<char>> &board);

	bool add_new_state(void);

	void solve(void);

	// void timed_print(const std::stack<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::stack<std::vector<Piece>> &pieces_queue);
	// void timed_print_core(const std::stack<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::stack<std::vector<Piece>> &pieces_queue);
	// std::chrono::duration<double> get_elapsed_seconds(void);

	void update_finished(void);

	void move_piece(cell_id &piece_index, piece_direction &direction, std::stack<std::tuple<cell_id, piece_direction, cell_id, piece_direction>> &pieces_stack);

	bool a_rect_cant_be_moved(const std::vector<Rect> &rects, const cell_id piece_index, const Pos &piece_top_left);
	bool is_invalid_move(const cell_id piece_index, const Pos &piece_pos);

	piece_direction get_inverted_direction(const piece_direction &direction);
	cell_id get_next_piece_index(const cell_id &piece_index, const piece_direction &direction);
	piece_direction get_next_direction(const cell_id &piece_index, const piece_direction &direction);

	void move(Pos &piece_pos, const piece_direction direction);
	bool move_doesnt_cross_puzzle_edge(const std::size_t piece_index, const Pos &piece_pos);
	bool no_intersection(const std::size_t piece_index_1, const Pos &piece_1_pos);
	const std::vector<Piece> deepcopy_pieces_positions(void);

	const std::string get_path_string(const std::vector<std::pair<std::size_t, char>> &path);

public:
	SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name);
	void run(void);
};
