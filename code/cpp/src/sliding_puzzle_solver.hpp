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

////////

enum class Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

////////

class SlidingPuzzleSolver
{
	// Constants
	int width;
	int height;

	std::map<std::string, StartingPieceInfo> starting_pieces_info;
	std::map<std::string, StartingPiece> starting_pieces;
	std::map<std::string, EndingPiece> ending_pieces;

	std::chrono::steady_clock::time_point start_time;

	char empty_character;

	std::map<Direction, char> direction_characters;

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
	std::map<std::string, EndingPiece> json_ending_piece_info_to_map(json j);

	void set_starting_pieces(void);
	void initialize_variable_fields(void);

	std::chrono::duration<double> get_elapsed_seconds(void);

	template <class T>
	void print_board(std::map<std::string, T> pieces);
	template <class T>
	std::vector<std::vector<char>> get_board(std::map<std::string, T> pieces);
	std::vector<std::vector<char>> get_2d_vector(void);

public:
	SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name);
	void run(void);
};
