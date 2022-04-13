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
#include <queue>
#include <thread>

////////

typedef int CellIndex;
typedef int PieceID;

////////

class SlidingPuzzleSolver
{
	// Constants
	int width;
	int height;

	std::vector<StartingPieceInfo> starting_pieces_info;
	std::vector<EndingPiece> ending_pieces;

	std::vector<Wall> walls;

	// std::unordered_map<CellIndex, PieceID> cells;
	// std::vector<CellIndex> active_cells;
	std::vector<std::vector<bool>> is_wall;
	// std::array<int, 4> movable_directions;

	const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

	const char empty_character = ' ';
	const char wall_character = '#';

	const std::vector<char> direction_characters = {'^', 'v', '<', '>'};

	const bool print_board_every_path = false;

	// Variables
	std::unordered_set<std::size_t> states;

	int state_count = 0;
	int prev_state_count = 0;
	bool finished = false;

	// Methods
	const json get_puzzle_json(std::filesystem::path &exe_path, const std::string &puzzle_name);
	const std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path &exe_path, const std::string &puzzle_name);

	void initialize_constant_fields(const json &puzzle_json);

	void set_starting_pieces_info(const json &starting_pieces_info_json);
	void set_ending_pieces(const json &starting_pieces_json);

	std::vector<Piece> get_starting_pieces(void);

	void set_width_and_height(const json &walls_json);
	void set_walls(const json &walls_json);
	void set_is_wall(void);
	// int get_index(int x, int y);

	template <class T>
	void print_board(const std::vector<T> &pieces);
	template <class T>
	const std::vector<std::vector<char>> get_board(const std::vector<T> &pieces);
	const std::vector<std::vector<char>> get_2d_vector(void);
	template <class T>
	void set_pieces_on_board(const std::vector<T> &pieces, std::vector<std::vector<char>> &board);
	void set_walls_on_board(std::vector<std::vector<char>> &board);

	bool add_new_state(const std::vector<Piece> &pieces);

	void solve(std::vector<Piece> starting_pieces);

	void timed_print(const std::queue<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::queue<std::vector<Piece>> &pieces_queue);
	void timed_print_core(const std::queue<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::queue<std::vector<Piece>> &pieces_queue);
	std::chrono::duration<double> get_elapsed_seconds(void);

	void update_finished(std::vector<Piece> &pieces);
	void move(const int direction, Pos &piece_pos);
	bool is_valid_move(const std::size_t piece_index, const Pos &piece_pos, const std::vector<Piece> &pieces);
	bool move_doesnt_cross_puzzle_edge(const std::size_t piece_index, const Pos &piece_pos);
	bool no_intersection(const std::size_t piece_index_1, const Pos &piece_1_pos, const std::vector<Piece> &pieces);
	const std::vector<Piece> deepcopy_pieces_positions(const std::vector<Piece> &pieces);

	const std::string get_path_string(const std::vector<std::pair<std::size_t, char>> &path);
	std::size_t get_pieces_hash(const std::vector<Piece> &pieces) const;

public:
	SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name);
	void run(void);
};
