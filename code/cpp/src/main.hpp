#pragma once

////////

#include "piece.hpp" // piece

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

	json pieces;
	json piece_ending_positions;

	std::chrono::steady_clock::time_point start_time;

	char empty_character;

	std::map<Direction, char> direction_characters;

	// Variables
	std::vector<Piece> states;

	// Methods
	json get_puzzle_json(std::filesystem::path exe_path, std::string puzzle_name);
	std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path exe_path, std::string puzzle_name);
	void initialize_constant_fields(json puzzle_json);
	void initialize_variable_fields(void);
	std::chrono::duration<double> get_elapsed_seconds(void);

public:
	SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name);
	void run(void);
};
