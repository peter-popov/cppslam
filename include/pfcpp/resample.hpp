#pragma once

#include "utils.hpp"
#include <random>
#include <vector>

namespace pfcpp
{

template<typename P>
auto get_weight(const P& p) -> decltype(P::weight)
{
	return p.weight;
}

template<typename T>
class MultimodalDistribution
{
public:	
	template<typename I>
	MultimodalDistribution(I from, I to)
	{
		acc.reserve(std::distance(from, to));
		T total = 0;
		for(; from != to; ++from)
		{
			total += get_weight(*from);
			acc.push_back(total);
		}
		last_iter = acc.begin();
	}

	T total() const { return acc.back(); }

	size_t draw(T rnd) const
	{
		auto pos = std::lower_bound(std::begin(acc), std::end(acc), rnd);
		if (pos == std::end(acc))
			return acc.size() - 1;
		return std::distance(std::begin(acc), pos);
	}

	size_t draw_next(T rnd) const
	{
		last_iter = std::lower_bound(last_iter, std::end(acc), rnd);
		if (last_iter == std::end(acc))
			return acc.size() - 1;
		return std::distance(std::begin(acc), last_iter);
	}	

private:
	std::vector<T> acc;
	mutable typename std::vector<T>::const_iterator last_iter;
};


template<typename P>
void resample(const std::vector<P>& in, std::vector<P>& out)
{

}
 


/**
 * Multimodal resampling
 */
template<typename P>
void multimodal_resample(const std::vector<P>& in, std::vector<P>& out)
{
	if (out.size() != in.size())
		out.resize(in.size());

	MultimodalDistribution<double> mmdist(in.begin(), in.end());
	
	std::uniform_real_distribution<double> unif(0., mmdist.total());
	std::default_random_engine re{std::random_device{}()};      
  
 	for (int i = 0; i < in.size(); ++i)
	{
		auto r = unif(re);
		out[i] = in[mmdist.draw(r)];
	}	
}


/**
 *
 */
 template<typename P>
 void stratified_resample(const std::vector<P>& in, std::vector<P>& out)
 {
 	if (out.size() != in.size())
		out.resize(in.size());

	MultimodalDistribution<double> mmdist(in.begin(), in.end());	

	std::uniform_real_distribution<double> unif(0., 1.0);
	std::default_random_engine re{std::random_device{}()};

	for (int i = 0; i < in.size(); ++i)
	{
		auto r = ((i-1) + unif(re))/in.size();
		out[i] = in[mmdist.draw(r)];
	}
 }

 /**
 *
 */
 template<typename P>
 void systematic_resample(const std::vector<P>& in, std::vector<P>& out)
 {
 	 if (out.size() != in.size())
		out.resize(in.size());

 	MultimodalDistribution<double> mmdist(in.begin(), in.end());	

	std::uniform_real_distribution<double> unif(0., 1.0);
	std::default_random_engine re{std::random_device{}()};

	auto u = unif(re);

 	for (int i = 0; i < in.size(); ++i)
	{
		auto r = ((i-1) + u)/in.size();
		out[i] = in[mmdist.draw_next(r)];
	}	
 }



}