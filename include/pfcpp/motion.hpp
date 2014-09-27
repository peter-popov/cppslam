#pragma once

#include <random>
#include <array>

namespace pfcpp
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




class VelocityMotionModelSampler
{
public:
	VelocityMotionModelSampler()
	: VelocityMotionModelSampler({0.0,0.0,0.0,0.0,0.0,0.0})
	{		
	}

	VelocityMotionModelSampler(std::array<double, 6>&& params)
	: a(std::forward<std::array<double, 6>>(params))
	, gen(std::random_device{}())
	, distr(0, 1.0)
	{		
	}

	template<typename State, typename Control>
	State operator()(const State& s, const Control& control, double dt = 1.0)
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
			xp += -dv_dw * (sin(teta) - sin(teta + w * dt));
			yp +=  dv_dw * (cos(teta) - cos(teta + w * dt));
		}
		else
		{
			xp += v * cos(teta);
			yp += v * sin(teta);	
		}
		auto dir = teta + w * dt;
 		return {xp, yp, dir};	 
	}


private:
	///noise params
	std::array<double, 6> a; 
	std::mt19937 gen;
	std::normal_distribution<> distr;
};


}