#pragma once

#include <random>
#include <array>

#include "utils.hpp"

namespace pfcpp
{


template<typename S>
double get_x(const S& p)
{
	return p.x;
}

template<typename S>
double get_y(const S& p)
{
	return p.y;
}

template<typename S>
double get_dir(const S& p)
{
	return p.direction;
}

template<typename C>
double get_v(const C& p)
{
	return p.v;
}

template<typename C>
double get_w(const C& p)
{
	return p.w;
}

template<typename C>
double get_t(const C&)
{
	return 1.0;
}


class VelocityMotionModelSampler
{
public:
	VelocityMotionModelSampler()
	: VelocityMotionModelSampler({0.0,0.0,0.0,0.0,0.0,0.0})
	{		
	}

	VelocityMotionModelSampler(std::array<double, 6>&& params)
	: a(std::forward<std::array<double, 6>>(params))
	{		
	}

	template<typename State, typename Control>
	State operator()(const State& s, const Control& control)
	{		
		// Noisy control
		auto v = get_v(control) + noise(a[0] * fabs(get_v(control)) + a[1] * fabs(get_w(control)));
		auto w = get_w(control) + noise(a[2] * fabs(get_v(control)) + a[3] * fabs(get_w(control)));
		auto gm = noise(a[4] * fabs(get_v(control)) + a[5] * fabs(get_w(control)));
		auto dt = get_t(control);

		// Get current state parameters
		auto heading = get_dir(s);
		auto x = get_x(s);
		auto y = get_y(s);

		auto dv_dw = v / w;
			
		if (std::isnormal(dv_dw))
		{
			x += -dv_dw * (std::sin(heading) - std::sin(heading + w * dt));
			y +=  dv_dw * (std::cos(heading) - std::cos(heading + w * dt));
		}
		else
		{
			x += v * std::cos(heading);
			y += v * std::sin(heading);	
		}
		heading += + w * dt + gm;
 		return {x, y, heading};	 
	}


private:
	///noise params
	std::array<double, 6> a;
	GaussianNoise<double> noise;
};


}