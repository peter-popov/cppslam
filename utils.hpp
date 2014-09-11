#pragma once

#include<random>

namespace mcl
{


template<typename T>
class GaussianNoise
{
public:
	GaussianNoise(T sigma)
	: gen(seed_rd())
	, distr(0, sigma)
	{
	}

	T operator()()
	{
		return static_cast<T>(std::round(distr(gen)));	
	}
private:
	std::random_device seed_rd;
	std::mt19937 gen;
	std::normal_distribution<> distr;
};


}