#pragma once

#include "utils.hpp"
#include <vector>

namespace pfcpp
{

template<typename P>
auto get_weight(const P& p) -> decltype(P::weight) {
	return p.weight;
}


template<typename P, typename Sampler>
void resample(const std::vector<P>& in, std::vector<P>& out, Sampler sampler)
{
	if (out.size() != in.size())
		out.resize(in.size());

	MultimodalDistribution<double> mmdist(in.begin(), in.end(), [](const auto& x){ return get_weight(x); });
	  
 	for (int i = 0; i < in.size(); ++i)
		out[i] = in[mmdist.draw(sampler(i))];
}

 
/**
* Multimodal resampling
*/
template<typename P, typename Gen = Uniform<double>>
void multimodal_resample(const std::vector<P>& in, std::vector<P>& out, Gen gen = Gen(0, 1.0)) {
	resample(in, out, [&](auto idx) { return gen(); });
}


/**
*
*/
template<typename P, typename Gen = Uniform<double>>
void stratified_resample(const std::vector<P>& in, std::vector<P>& out, Gen gen = Gen(0, 1.0)) {
	resample(in, out, [&](auto idx) { return (idx + gen()) / in.size(); });
}


/**
*
*/
template<typename P, typename Gen = Uniform<double>>
void systematic_resample(const std::vector<P>& in, std::vector<P>& out, Gen gen = Gen(0, 1.0)) {
	resample(in, out, [&, sample = gen()](auto idx) { return (idx + sample) / in.size(); });
}

} //pfcpp