#pragma once

#include <random>

namespace pfcpp
{

static const double Pi = 3.14159265359;

template<typename T>
class GaussianNoise
{
public:
	GaussianNoise(T sigma = 1.0)
	: gen(std::random_device{}())
	, distr(0, sigma)
	{
	}

	T operator()()
	{
		return static_cast<T>(distr(gen));	
	}

	T operator()(T other_sigma)
	{
		return static_cast<T>(distr(gen, typename decltype(distr)::param_type(0., other_sigma)));
	}


private:
	std::mt19937 gen;
	std::normal_distribution<> distr;
};



template<typename I, typename Setter, typename Getter>
void normalize(I start, I end, Setter set, Getter get)
{
	decltype(set(*start)) total = 0;
	for (auto p = start; p != end; ++p)
		total += get(*p);
	for (auto p = start; p != end; ++p)
		set(*p, get(*p) / total);
}

template<typename I>
void normalize(I start, I end, decltype(std::declval<I>()->weight) dummy = 0)
{
	decltype(std::declval<I>()->weight) total = 0;
	for (auto p = start; p != end; ++p)
		total += p->weight;
	for (auto p = start; p != end; ++p)
		p->weight /= total;
}

}