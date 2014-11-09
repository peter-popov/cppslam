#pragma once
#include <cmath>
#include <array>
#include "utils.hpp"

namespace pfcpp
{

struct SensorSetings
{
	double ray_length;
	std::array<double, 4> a;
	double sigma;
	double lambda;
};

class BeamSensorModel
{
	SensorSetings m_settings;

public:

	BeamSensorModel(SensorSetings settings)
	: m_settings(settings)
	{
	}

	template<typename Measurment>
	auto operator()(const Measurment& measured, const Measurment& expected)	
	{		
		auto q = 1.0;
		auto z_pos_e = std::begin(expected);
		auto z_pos = std::begin(measured);

		for (; z_pos != std::end(measured) && z_pos_e != std::end(expected); ++z_pos, ++z_pos_e)
		{
			q *= this->operator()(*z_pos, *z_pos_e);
		}
		return q;
	}

	auto operator()(const double& measured, const double& expected)	
	{		
		auto p = m_settings.a[0] * normal_measurment(measured, expected) + 
				 m_settings.a[1] * failure() +
				 m_settings.a[2] * error(measured) +
				 m_settings.a[3] * dynamic_objects(measured, expected);
		return std::isnan(p) ? 0.0 : p;
	}

private:

	double normal_measurment(double z, double ze) const
	{
		if ( z > m_settings.ray_length || ze > m_settings.ray_length )
			return 0.0;
		double sigma2 = m_settings.sigma;
		double sqrt2sigma = std::sqrt(2*sigma2);
		
		auto p = std::exp(-0.5 * (z - ze) * (z - ze) / sigma2) / sqrt2sigma;
		auto norm = std::erf((m_settings.ray_length - ze) / sqrt2sigma) + std::erf(ze / sqrt2sigma);
		return p / norm;	
	}

	double failure() const
	{
		return 1 / m_settings.ray_length;
	}

	double error(double z) const
	{
		if (fabs(z - m_settings.ray_length) < 0.001)
			return 1.0;
		else
			return 0.0;
	}

	double dynamic_objects(double z, double ze) const
	{
		if (z >= ze)
			return 0.0;
		else
			return m_settings.lambda * exp(-m_settings.lambda * z) * (1 - exp(-m_settings.lambda * ze));
	}
};

} //pfcpp