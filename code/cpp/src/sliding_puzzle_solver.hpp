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
#include <unordered_set>		// unordered_set
#include <queue>	// queue
#include <thread>	// this_thread

////////

class SlidingPuzzleSolver
{
	// Constants
	int width;
	int height;

	std::map<std::string, StartingPieceInfo> starting_pieces_info;
	std::map<std::string, Piece> starting_pieces;
	std::map<std::string, Piece> ending_pieces;

	const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

	const char empty_character = ' ';

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
	void set_starting_pieces(void);
	void set_ending_pieces(const json &ending_pieces_json);

	template <class T>
	void print_board(const std::map<std::string, T> &pieces);
	template <class T>
	const std::vector<std::vector<char>> get_board(const std::map<std::string, T> &pieces);
	const std::vector<std::vector<char>> get_2d_vector(void);

	bool add_new_state(const std::map<std::string, Piece> &pieces);

	void solve(void);
	void timed_print(const std::queue<std::map<std::string, Piece>> &pieces_queue);
	std::chrono::duration<double> get_elapsed_seconds(void);

	void update_finished(std::map<std::string, Piece> &pieces);
	void move(const int direction, Pos &piece_pos);
	bool is_valid_move(const std::string &piece_label, const Pos &piece_pos, const std::map<std::string, Piece> &pieces);
	bool move_doesnt_cross_puzzle_edge(const std::string &piece_label, const Pos &piece_pos);
	bool no_intersection(const std::string &piece_label_1, const Pos &piece_1_pos, const std::map<std::string, Piece> &pieces);
	const std::map<std::string, Piece> deepcopy_pieces_positions(const std::map<std::string, Piece> &pieces);

	const std::string get_path_string(const std::vector<std::pair<std::string, char>> &path);
	std::size_t get_pieces_hash(const std::map<std::string, Piece> &pieces) const;

public:
	SlidingPuzzleSolver(std::filesystem::path &exe_path, const std::string &puzzle_name);
	void run(void);
};
