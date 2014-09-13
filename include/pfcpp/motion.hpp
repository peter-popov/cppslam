#pragma once

#include <random>
#include <array>
#include <tuple>

namespace mcl
{


template<typename S>
auto get_x(const S& p) -> decltype(S::x)
{
	return p.x;
}

template<typename S>
auto get_y(const S& p) -> decltype(S::y)
{
	return p.y;
}

template<typename S>
auto get_dir(const S& p) -> decltype(S::direction)
{
	return p.direction;
}

template<typename C>
auto get_v(const C& p) -> decltype(C::v)
{
	return p.v;
}

template<typename C>
auto get_w(const C& p) -> decltype(C::w)
{
	return p.w;
}




template<typename State, typename Control>
class VelocityMotionModelSampler
{
public:
	VelocityMotionModelSampler()
	: VelocityMotionModelSampler({0.001,0.5,0.02,0.5,0.1,0.1})
	{		
	}

	VelocityMotionModelSampler(std::array<double, 6>&& params)
	: a(std::forward<std::array<double, 6>>(params))
	, gen(seed_rd())
	, distr(0, 1.0)
	, dt(1)
	{		
	}

	State operator()(const State& s, const Control& control)
	{
		auto sample = [&](auto x){ return distr(gen, typename decltype(distr)::param_type(0., x));};

		auto v = get_v(control) + sample(a[0] * fabs(get_v(control)) + a[1] * fabs(get_w(control)));
		auto w = get_w(control) + sample(a[2] * fabs(get_v(control)) + a[3] * fabs(get_w(control)));
		auto gm = sample(a[4] * fabs(get_v(control)) + a[5] * fabs(get_w(control)));
	
		
		auto teta = get_dir(s);
		auto xp = get_x(s);
		auto yp = get_y(s);
		
		if (fabs(w) > 0.00001)
		{
			auto dv_dw = v / w;
			xp -= dv_dw * (sin(teta) - sin(teta + w * dt));
			yp -= dv_dw * (cos(teta) - cos(teta + w * dt));
		}
		else
		{
			xp += v * cos(teta);
			yp += -v * sin(teta);	
		}
		auto dir = teta + (w + gm) * dt;
		return {int(xp),- int(yp), dir};	 
	}


private:
	///noise params
	std::array<double, 6> a; 

	std::random_device seed_rd;
	std::mt19937 gen;
	std::normal_distribution<> distr;
	double dt;
};


}