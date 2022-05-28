#include "timed_printer.hpp"


void TimedPrinter::timed_print(const pieces_queue_t &pieces_queue, const path_queue_t &path_queue)
{
	std::cout << std::endl;

	while (!sps.finished)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		timed_print_core(pieces_queue, path_queue);
	}

	std::cout << std::endl << std::endl << "Path:" << std::endl << get_path_string(path_queue.front()) << std::endl << std::endl;
}


void TimedPrinter::timed_print_core(const pieces_queue_t &pieces_queue, const path_queue_t &path_queue)
{
	// TODO: Store elapsed_time in something more appropriate than int.
	const int elapsed_time = get_elapsed_seconds().count();

	const int states_count_diff = sps.state_count - sps.prev_state_count;
	sps.prev_state_count = sps.state_count;

	std::cout << "\33[2K\r"; // Clears the line and goes back to the left.

	std::cout << "Elapsed time: " << elapsed_time << " seconds";

	KiloFormatter kf;

	if (path_queue.size() > 0)
	{
		std::cout << ", Path length: " << kf.format(path_queue.front().size());
	}

	std::cout << ", Unique states: " << kf.format(sps.state_count) << " (+" << kf.format(states_count_diff) << "/s)";

	std::cout << ", Queue length: " << kf.format(pieces_queue.size());

	std::cout << std::flush;
}


std::chrono::duration<double> TimedPrinter::get_elapsed_seconds(void)
{
	// TODO: Cast the result to seconds in type double, cause idk how this works.
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return end_time - sps.start_time;
}


std::string TimedPrinter::get_path_string(const path_t &path)
{
	std::stringstream path_stringstream;

	for (path_t::const_iterator pair_it = path.cbegin(); pair_it != path.cend(); ++pair_it)
	{
		std::size_t piece_index = pair_it->first;
		path_stringstream << sps.piece_labels[piece_index];

		char direction = pair_it->second;
		path_stringstream << sps.direction_characters[direction];
	}

	return path_stringstream.str();
}
