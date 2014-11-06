#pragma once

#include <vector>

#include "resample.hpp"
#include "motion.hpp"
#include "utils.hpp"


namespace pfcpp
{

/**
 * Particle representation
 */
template<typename State>
struct Particle
{
	State state;
	double weight;
};


/**
 * Implementation of the Particle Filter
 */
template<typename State, 
		 typename MotionModel = VelocityMotionModelSampler>
struct ParticleFilter
{
	ParticleFilter() {}
	ParticleFilter(std::vector<State> states, const MotionModel& model)
	: motion_model(model)
	{
		for (auto& s: states)
		{
			particles.push_back({s,1.0});
		}
    }

	template<typename SensorUpdate, typename Control>
	void operator()(SensorUpdate sensor_update, const Control& control)
	{
		for(auto& p: particles) 
		{
			p.state = motion_model(p.state, control);	
			p.weight = sensor_update(p.state);
		}
		normalize(particles.begin(), particles.end());
		systematic_resample(particles, tmp_buffer);
		std::swap(particles, tmp_buffer);
	}

	std::vector<Particle<State>> particles;

	MotionModel motion_model;	
	std::vector<Particle<State>> tmp_buffer;		
};

}
