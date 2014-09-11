#pragma once

#include <vector>
#include <random>

#include "resample.hpp"

using namespace std;

namespace mcl
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
    double total = 0;
    for(auto& p: v) total += p.weight;
    for(auto& p: v) p.weight = p.weight / total;
}


template<typename S>
void scale(std::vector<Particle<S>>& v)
{
    double max = 0;
    for(auto& p: v) if (p.weight > max) max = p.weight;
    for(auto& p: v) p.weight = p.weight / max;
}



template<typename T>
double weight_function(vector<T> expected, vector<T> measured)
{
	double res = 0;
	auto p1 = begin(expected);
	auto p2 = begin(measured);
	for( ;p1 != end(expected) && p2 != end(measured);
		 ++p1, ++p2)
	{
		res += (*p2 - *p1)*(*p2 - *p1);
	}
	return 1.0/(1 + sqrt(res));
}


template<typename State>
struct MCL
{
	MCL() {}
	MCL(vector<State> states)
	{
		for (auto& s: states)
		{
			particles.push_back({s,1.0});
		}
        normilize(particles);
	}

	MCL(initializer_list<State> states)
	{
		for (auto& s: states)
		{
			particles.push_back({s,1.0});
		}
	}

	template<typename Measurment, typename Action>
	void operator()(Measurment measurment, Action action)
	{
		for(auto& p: particles) 
		{
			p.state = action(p.state);	
			p.weight = measurment(p.state);
		}

		normilize(particles);
		particles = resample(particles);
	}

	vector<Particle<State>> particles;		
};

}
