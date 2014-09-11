#pragma once

#include "utils.hpp"
#include <random>
#include <vector>

namespace mcl
{

template<typename P>
auto get_weight(const P& p) -> decltype(P::weight)
{
	return p.weight;
}

template<typename P>
std::vector<P> resample(std::vector<P>& v)
{
	std::vector<double> acc;
	for(auto& p: v) acc.push_back(get_weight(p));
	for (int i = 1; i < acc.size(); ++i) acc[i] += acc[i-1];

	std::uniform_real_distribution<double> unif(0., acc.back());
	std::default_random_engine re{std::random_device{}()};      
  
 	std::vector<P> res;
 	for (int i = 0; i < acc.size(); ++i)
	{
		double r = unif(re);
		int idx = 0;
		for (; idx < acc.size( ); ++idx)
			if (acc[idx] >= r) break;
		res.push_back(v[idx]);
	}	
	return res;	
}


}