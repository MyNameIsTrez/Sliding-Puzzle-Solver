// #include "main.hpp"

////////

#include "json.hpp" // json
using json = nlohmann::json;

////////

#include <iostream> // std::cout, std::endl
#include <fstream>	// ifstream

////////

std::filesystem::path get_puzzle_path_from_exe_path(std::filesystem::path exe_path)
{
	std::filesystem::path exe_dir_path = exe_path.remove_filename();
	std::filesystem::path puzzle_path = exe_dir_path / "puzzles" / "klotski.jsonc";
	return puzzle_path;
}

json get_puzzle_json(std::filesystem::path puzzle_path)
{
	std::ifstream f(puzzle_path);
	json j = json::parse(f,
						 /* callback */ nullptr,
						 /* allow exceptions */ true,
						 /* ignore_comments */ true);
	return j;
}

int main(int argc, char *argv[])
{
	(void)argc;

	std::filesystem::path exe_path = argv[0];

	std::filesystem::path puzzle_path = get_puzzle_path_from_exe_path(exe_path);

	json j = get_puzzle_json(puzzle_path);

	std::cout << j.dump(4) << std::endl;

	return (EXIT_SUCCESS);
}
