#include "../sliding_puzzle_solver.hpp"

void SlidingPuzzleSolver::timed_print(const std::queue<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::queue<std::vector<Piece>> &pieces_queue)
{
	std::cout << std::endl;

	// TODO: Try to get rid of either the running or finished field.
	// The reason we have them both right now has to do with the last
	while (!finished)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		timed_print_core(path_queue, pieces_queue);
	}

	std::cout << std::endl << std::endl << "Path:" << std::endl;
	std::cout << get_path_string(path_queue.front()) << std::endl << std::endl;
}

void SlidingPuzzleSolver::timed_print_core(const std::queue<std::vector<std::pair<std::size_t, char>>> &path_queue, const std::queue<std::vector<Piece>> &pieces_queue)
{
	// TODO: Store elapsed_time in something more appropriate than int.
	const int elapsed_time = get_elapsed_seconds().count();

	const int states_count_diff = state_count - prev_state_count;
	prev_state_count = state_count;

	std::cout << "\33[2K\r"; // Clears the line and goes back to the left.

	std::cout << "Elapsed time: " << elapsed_time << " seconds";

	KiloFormatter kf;

	if (path_queue.size() > 0)
	{
		const std::size_t path_length = path_queue.front().size();
		std::cout << ", Path length: " << kf.format(path_length);
	}

	std::cout << ", Unique states: " << kf.format(state_count) << " (+" << kf.format(states_count_diff) << "/s)";
	std::cout << ", Queue length: " << kf.format(pieces_queue.size());
	std::cout << std::flush;
}

std::chrono::duration<double> SlidingPuzzleSolver::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - start_time;
}
