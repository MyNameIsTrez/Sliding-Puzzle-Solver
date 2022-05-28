#pragma once


#include "../typedefs.hpp"


class SlidingPuzzleSolver;


#include "../sliding_puzzle_solver.hpp"


#include <iostream>


class BoardPrinter
{
public:
	BoardPrinter(SlidingPuzzleSolver &sps_) : sps(sps_) {};
	void print_board(const pieces_t &pieces);

private:
	const std::vector<std::vector<char>> get_board(const pieces_t &pieces);
	const std::vector<std::vector<char>> get_2d_vector(void);
	void set_pieces_on_board(std::vector<std::vector<char>> &board, const pieces_t &pieces);
	void set_walls_on_board(std::vector<std::vector<char>> &board);

	SlidingPuzzleSolver &sps;
};
