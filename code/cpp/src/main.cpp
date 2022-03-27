// #include "main.hpp"

// #include <iostream> // std::cout, std::endl
// #include <stdlib.h> // EXIT_SUCCESS

// int main(void)
// {
// 	std::cout << "Hello World!" << std::endl;
// 	return (EXIT_SUCCESS);
// }

#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

int main()
{
	std::string s = R"(
    {
        // update in 2006: removed Pluto
        "planets": ["Mercury", "Venus", "Earth", "Mars",
                    "Jupiter", "Uranus", "Neptune" /*, "Pluto" */]
    }
    )";

	try
	{
		json j = json::parse(s);
	}
	catch (json::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	json j = json::parse(s,
						 /* callback */ nullptr,
						 /* allow exceptions */ true,
						 /* ignore_comments */ true);
	std::cout << j.dump(2) << '\n';
}
