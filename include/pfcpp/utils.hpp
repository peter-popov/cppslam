#pragma once

#include<random>

namespace pfcpp
{


template<typename T>
class GaussianNoise
{
public:
	GaussianNoise(T sigma)
	: gen(std::random_device{}())
	, distr(0, sigma)
	{
	}

	T operator()()
	{
		return static_cast<T>(distr(gen));	
	}
private:
	std::mt19937 gen;
	std::normal_distribution<> distr;
};


}