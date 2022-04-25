#pragma once

////////

#include <sstream>      // std::stringstream

////////

class KiloFormatter
{
public:
	KiloFormatter() {}

	const std::string format(double nbr)
	{
		std::stringstream formatted;

		if (nbr >= 1e9)
		{
			formatted << shorten(nbr / 1e9) << " G";
		}
		else if (nbr >= 1e6)
		{
			formatted << shorten(nbr / 1e6) << " M";
		}
		else if (nbr >= 1e3)
		{
			formatted << shorten(nbr / 1e3) << " k";
		}
		else
		{
			formatted << nbr;
		}

		return formatted.str();
	}
private:
	/*
	Outputs strings in this format:
	1.234
	12.34
	123.4
	*/
	std::string shorten(double nbr)
	{
		return std::to_string(nbr).substr(0, 5);
	}
};
