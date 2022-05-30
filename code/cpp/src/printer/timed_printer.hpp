#pragma once


#include "../typedefs.hpp"


#include <chrono>
#include <iostream>


class SlidingPuzzleSolver;

class TimedPrinter
{
public:
	TimedPrinter(SlidingPuzzleSolver &sps_) : sps(sps_) {};
	void timed_print(const pieces_queue_t &pieces_queue, const path_queue_t &path_queue);

private:
	void timed_print_core(const pieces_queue_t &pieces_queue, const path_queue_t &path_queue);
	std::chrono::duration<double> get_elapsed_seconds(void);
	std::string get_path_string(const path_t &path);

	const SlidingPuzzleSolver &sps;
};
