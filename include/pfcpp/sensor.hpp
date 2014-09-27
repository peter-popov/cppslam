#pragma once
#include <cmath>
#include <iterator>
#include <iostream>
#include <array>

namespace pfcpp
{

class BeamSensorModel
{
public:
	const double Pi = 3.14159;
	const double max_range = 1000;
	std::array<double, 4> a {0.2, 0.2, 0.7, 0.0};

	double normal_measurment(double z, double ze) const
	{
		if ( z > max_range || ze > max_range )
				return 0.0;
		long double sigma2 = 1000.0;
		long double sqrt2sigma = std::sqrt(2*sigma2);
		
		auto p = std::exp(-0.5 * (z - ze) * (z - ze) / sigma2);
		if (std::isnan(p)) p = 0.0;
		auto norm = std::erf((max_range - ze)/sqrt2sigma) - std::erf(-ze/sqrt2sigma);
		return 2 * p / norm;	
	}

	double failure() const
	{
		return 1 / max_range;
	}

	double error(double z) const
	{
		if (fabs(z - max_range) < 0.001)
			return 1.0;
		else
			return 0.0;
	}

	long double dynamic_objects(double z) const
	{
		return 0.0;
	}


	template<typename Measurment>
	double operator()(const Measurment& measured, const Measurment& expected)	
	{
		auto q = 0.0;
		auto z_pos_e = std::begin(expected);
		auto z_pos = std::begin(measured);

		for (; z_pos != std::end(measured) && z_pos_e != std::end(expected); ++z_pos, ++z_pos_e)
		{
			auto p = a[0] * normal_measurment(*z_pos, *z_pos_e) + 
				 	 a[1] * failure() +
				 	 a[2] * error(*z_pos) +
				 	 a[3] * dynamic_objects(*z_pos);
			q *= p;
		}
		return std::isnan(q) ? 0.0 : q;
	}

};

} //pfcpp