#include "mcl.hpp"
#include "mazeworld.hpp"
#include "flatworld.hpp"
#include <boost/geometry.hpp>
#include <iostream>
#include <cmath>

using namespace std;

namespace world = flatworld;

int main()
{
	world::Scene scene;

	{
		world::Polygon poly;
	    boost::geometry::read_wkt(
	        "POLYGON((0.0 0.0, 0.0 10.0, 1.0 10.0, 1.0 0.0))", poly);
	    scene.add(std::move(poly));
	}

	{
		world::Polygon poly;
	    boost::geometry::read_wkt(
	        "POLYGON((9.0 0.0, 9.0 10.0, 10.0 10.0, 10.0 0.0))", poly);
	    scene.add(std::move(poly));
	}

	auto r = scene.measure_from({5.0, 5.0});
	for (auto x: r)
		std::cout << x << " ";
	std::cout << std::endl;
}