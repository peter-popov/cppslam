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
		if (std::isnormal(dv_dw))
		{
			return { get_x(pose) - dt * dv_dw * (std::sin(heading) - std::sin(heading + noisy_w * dt)),
					 get_y(pose) + dt * dv_dw * (std::cos(heading) - std::cos(heading + noisy_w * dt)),
					 heading + w * dt + gm };	
		}
		else
		{
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
	OdometryMotionModelSampler()
	: OdometryMotionModelSampler({0.0,0.0,0.0,0.0,0.0,0.0}) {		
	}

	OdometryMotionModelSampler(std::array<double, 6>&& params)
	: a(std::forward<std::array<double, 6>>(params)) {		
	}

	template<typename Pose>
	Pose operator()(const Pose& start, const Pose& end) {		
		return { 0, 0, 0};			
	} 

private:
	///noise params
	std::array<double, 6> a;
	GaussianNoise<double> noise;
};


}