#pragma once

#include <vector>

#include "resample.hpp"
#include "motion.hpp"


namespace pfcpp
{

template<typename State>
struct Particle
{
	State state;
	double weight;
};


template<typename S>
void normilize(std::vector<Particle<S>>& v)
{
    decltype(Particle<S>::weight) total = 0;
    for(auto& p: v) total += p.weight;
    for(auto& p: v) p.weight = p.weight / total;
}


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

		normilize(particles);
		//
		// @todo: think about implace resampling
		particles = stratified_resample(particles);
	}

	MotionModel motion_model;
	std::vector<Particle<State>> particles;		
};

}
