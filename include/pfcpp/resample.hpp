#pragma once

#include "utils.hpp"
#include <vector>

namespace pfcpp
{

template<typename P>
auto get_weight(const P& p) -> decltype(P::weight)
{
	return p.weight;
}


template<typename P, typename Sampler>
void resample(const std::vector<P>& in, std::vector<P>& out, Sampler sampler)
{
	if (out.size() != in.size())
		out.resize(in.size());

	MultimodalDistribution<double> mmdist(in.begin(), in.end());
	  
 	for (int i = 0; i < in.size(); ++i)
		out[i] = in[mmdist.draw(sampler(i))];
}

 
/**
* Multimodal resampling
*/
template<typename P>
void multimodal_resample(const std::vector<P>& in, std::vector<P>& out)
{
	Uniform<double> unif(0, 1.0);
	resample(in, out, [&](auto idx){ return unif();});
}


/**
*
*/
template<typename P>
void stratified_resample(const std::vector<P>& in, std::vector<P>& out)
{
	Uniform<double> unif(0, 1.0);
	resample(in, out, [&](auto idx){ return ((idx - 1) + unif()) / in.size();});
}

/**
*
*/
template<typename P>
void systematic_resample(const std::vector<P>& in, std::vector<P>& out)
{
	Uniform<double> unif(0, 1.0);	
	resample(in, out, [&, sample = unif()] (auto idx) {return ((idx - 1) + sample) / in.size();});
}

} //pfcpp