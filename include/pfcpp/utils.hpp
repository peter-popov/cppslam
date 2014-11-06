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


template<typename T>
class Uniform
{
public:
	Uniform(T a = 0.0, T b = 1.0)
	: gen(std::random_device{}())
	, distr(a, b)
	{
	}

	T operator()()
	{
		return static_cast<T>(distr(gen));	
	}

private:
	std::default_random_engine gen;
	std::uniform_real_distribution<T> distr;
};


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