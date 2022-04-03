#pragma once

////////

#include "pieces.hpp" // piece

////////

#include "json.hpp" // json
using json = nlohmann::json;

////////

// TODO: Use the Include What You Use extension so that redundant
// includes are automatically removed and these comments can be removed.

#include <iostream> // std::cout, std::endl
#include <fstream>	// ifstream
#include <chrono>	// steady_clock::now(), ::duration()
#include <vector>	// vector
#include <set>		// set
#include <queue>	// queue

////////

class SlidingPuzzleSolver
{
	// Constants
	int width;
	int height;

	std::map<std::string, StartingPieceInfo> starting_pieces_info;
	std::map<std::string, Piece> starting_pieces;
	std::map<std::string, Piece> ending_pieces;

	std::chrono::steady_clock::time_point start_time;

	char empty_character;

	std::vector<char> direction_characters;

	bool print_board_every_path;

	// Variables
	std::set<std::map<std::string, Piece>> states;

	int state_count;
	int prev_state_count;
	bool running;
	bool finished;

	// Methods
	json get_puzzle_json(std::filesystem::path exe_path, std::string puzzle_name);
	std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path exe_path, std::string puzzle_name);

	void initialize_constant_fields(json puzzle_json);

	std::map<std::string, StartingPieceInfo> json_starting_piece_info_to_map(json j);
	std::map<std::string, Piece> json_ending_piece_to_map(json j);

	void set_starting_pieces(void);
	void initialize_variable_fields(void);

	std::chrono::duration<double> get_elapsed_seconds(void);

	template <class T>
	void print_board(std::map<std::string, T> pieces);
	template <class T>
	std::vector<std::vector<char>> get_board(std::map<std::string, T> pieces);
	std::vector<std::vector<char>> get_2d_vector(void);

	bool add_new_state(std::map<std::string, Piece> pieces);

	void solve(void);
	void solve_and_print_path(void);

	void update_finished(std::map<std::string, Piece> pieces);
	void move(int direction, Pos &piece_pos);
	bool is_valid_move(std::string piece_label, Pos piece_pos, std::map<std::string, Piece> pieces);
	bool move_doesnt_cross_puzzle_edge(std::string piece_label, Pos piece_pos);
	bool no_intersection(std::string piece_label_1, Pos piece_1_pos, std::map<std::string, Piece> pieces);
	std::map<std::string, Piece> deepcopy_pieces_positions(std::map<std::string, Piece> pieces);

public:
	SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name);
	void run(void);
};
