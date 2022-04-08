#pragma once

////////

class KiloFormatter
{
public:
	KiloFormatter(const int digits) : power(std::pow(10, digits)-1) {}

	const std::string format(double nbr)
	{
		std::stringstream formatted;

		// formatted << std::fixed << std::setprecision(3);

		if (nbr >= 1e9)
		{
			formatted << round_to_n_digits(nbr / 1e9) << " G";
		}
		else if (nbr >= 1e6)
		{
			formatted << round_to_n_digits(nbr / 1e6) << " M";
		}
		else if (nbr >= 1e3)
		{
			formatted << round_to_n_digits(nbr / 1e3) << " k";
		}
		else
		{
			formatted << nbr;
		}

		return formatted.str();
	}
private:
	int power;

	/*
	Turn this into something like this:
	function y(n){
		const w = Math.floor(Math.log10(n)) - 3;
		return (n / (10 ** w));
	}
	*/
	double round_to_n_digits(double nbr)
	{
		return std::round(nbr * power) / power;
	}
};
