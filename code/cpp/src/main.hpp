#pragma once

////////

#include "json.hpp" // json
using json = nlohmann::json;

////////

#include <iostream> // std::cout, std::endl
#include <fstream>	// ifstream

////////

class SlidingPuzzleSolver
{

public:
	SlidingPuzzleSolver(std::filesystem::path exe_path, std::string puzzle_name);
	std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path exe_path, std::string puzzle_filename);
	json get_puzzle_json(std::filesystem::path puzzle_path);
	void Run(void);
};
