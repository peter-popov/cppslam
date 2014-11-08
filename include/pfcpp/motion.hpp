#pragma once

#include <random>
#include <array>

#include "utils.hpp"
#include "types.hpp"

namespace pfcpp
{


template<typename Control>
double get_velocity(const Control& c) {
	return c.v;
}

template<typename Control>
double get_rotation(const Control& p) {
	return p.w;
}

template<typename Control>
double get_time(const Control& c) {
	return c.time;
}

/**
 * Velocity based model
 */
class VelocityMotionModelSampler {
public:
	VelocityMotionModelSampler()
	: VelocityMotionModelSampler({0.0,0.0,0.0,0.0,0.0,0.0}) {		
	}

	VelocityMotionModelSampler(std::array<double, 6>&& params)
	: a(std::forward<std::array<double, 6>>(params)) {		
	}

	template<typename Pose, typename Control>
	Pose operator()(const Pose& pose, const Control& control) {		
		auto v = get_velocity(control);
		auto w = get_rotation(control);
		auto t = get_time(control);
		// Noisy control
		auto noisy_v = v + noise(a[0] * v + a[1] * w);
		auto noisy_w = w + noise(a[2] * v + a[3] * w);
		auto gm = noise(a[4] * v + a[5] * w);
		auto dt = get_time(control);

		auto heading = get_heading(pose);
		auto dv_dw = noisy_v / noisy_w;			
		if (std::isnormal(dv_dw)) {
			return { get_x(pose) - dt * dv_dw * (std::sin(heading) - std::sin(heading + noisy_w * dt)),
					 get_y(pose) + dt * dv_dw * (std::cos(heading) - std::cos(heading + noisy_w * dt)),
					 heading + w * dt + gm };	
		}
		else {
			return { get_x(pose) + dt * noisy_v * std::cos(heading),
					 get_y(pose) + dt * noisy_v * std::sin(heading),
					 heading + w * dt + gm };	
		}
	} 

private:
	///noise params
	std::array<double, 6> a;
	GaussianNoise<double> noise;
};


/**
 * Odometry model
 */
class OdometryMotionModelSampler {
public:

	typedef std::array<double, 4> NoiseParams;

	OdometryMotionModelSampler()
	: OdometryMotionModelSampler({0.0,0.0,0.0,0.0}) {		
	}

	OdometryMotionModelSampler(const NoiseParams& params)
	: a(params) {		
	}

	OdometryMotionModelSampler(NoiseParams&& params)
	: a(std::forward<NoiseParams>(params)) {		
	}

	template<typename Pose>
	Pose operator()(const Pose& start, const Pose& end) {		
	
		auto dx = get_x(end) - get_x(start);
		auto dy = get_y(end) - get_y(start);
		
		auto start_rotation = std::atan2(dy, dx) - get_heading(start);
		auto transition = std::sqrt(dx * dx + dy * dy);
		auto end_rotation = get_heading(end) - get_heading(start) - start_rotation;
		//
		//Apply noise
		double params[] = { a[0] * start_rotation + a[1] * transition,
						    a[2] * transition + a[3] * (start_rotation + end_rotation),
						    a[0] * end_rotation + a[1] * transition };
		
		start_rotation -= noise(params[0]);
		transition -= noise(params[1]);
		end_rotation -= noise(params[2]);

		return { get_x(start) + transition * std::cos(get_heading(start) + start_rotation), 
				 get_y(start) + transition * std::sin(get_heading(start) + start_rotation),
				 get_heading(start) + start_rotation + end_rotation };		
	} 

private:
	///noise params
	NoiseParams a;
	GaussianNoise<double> noise;
};


}