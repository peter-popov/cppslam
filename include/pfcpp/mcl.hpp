#pragma once

#include <vector>

#include "types.hpp"
#include "resample.hpp"
#include "motion.hpp"
#include "utils.hpp"


namespace pfcpp
{

/**
 * Implementation of the Particle Filter
 */
template<typename State>
struct ParticleFilter {
	ParticleFilter() {}
	ParticleFilter(std::vector<State> states) {
		for (auto& s: states)
			particles.push_back({s,1.0});
    }

	template<typename SensorUpdate, typename MotionUpdate>
	void operator()(SensorUpdate sensor_update, const MotionUpdate& motion_update) {
		for(auto& p: particles) {
			p.state = motion_update(p.state);	
			p.weight = sensor_update(p.state);
		}
		normalize(particles.begin(), particles.end());
		stratified_resample(particles, tmp_buffer);
		std::swap(particles, tmp_buffer);
	}

	std::vector<Particle<State>> particles;
	std::vector<Particle<State>> tmp_buffer;		
};

}
