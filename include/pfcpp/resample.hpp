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


/**
 * Multimodal resampling
 */
template<typename P>
std::vector<P> resample(std::vector<P>& v)
{
	MultimodalDistribution<double> mmdist(v.begin(), v.end());
	
	std::uniform_real_distribution<double> unif(0., mmdist.total());
	std::default_random_engine re{std::random_device{}()};      
  
 	std::vector<P> res;
 	for (int i = 0; i < v.size(); ++i)
	{
		auto r = unif(re);
		res.push_back(v[mmdist.draw(r)]);
	}	
	return res;	
}


/**
 *
 */
 template<typename P>
 std::vector<P> stratified_resample(std::vector<P>& v)
 {
	MultimodalDistribution<double> mmdist(v.begin(), v.end());	

	std::uniform_real_distribution<double> unif(0., 1.0);
	std::default_random_engine re{std::random_device{}()};

	std::vector<P> res;
 	for (int i = 0; i < v.size(); ++i)
	{
		auto r = ((i-1) + unif(re))/v.size();
		res.push_back(v[mmdist.draw_next(r)]);
	}	
	return res;	
 }

 /**
 *
 */
 template<typename P>
 std::vector<P> systematic_resample(std::vector<P>& v)
 {
 	MultimodalDistribution<double> mmdist(v.begin(), v.end());	

	std::uniform_real_distribution<double> unif(0., 1.0);
	std::default_random_engine re{std::random_device{}()};
	auto u = unif(re);

	std::vector<P> res;
 	for (int i = 0; i < v.size(); ++i)
	{
		auto r = ((i-1) + u)/v.size();
		res.push_back(v[mmdist.draw_next(r)]);
	}	
	return res;	
 }



}