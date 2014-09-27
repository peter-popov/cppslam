#pragma once

#include <map/shapes_map.hpp>
#include <utility>
#include <fstream>

namespace flatworld
{

using namespace pfcpp::maps;

double Pi = boost::math::constants::pi<double>();

struct Pose
{
	double x, y;
	double direction;

	operator ShapesMap::Point() const {return {x,y};}
	operator ShapesMap::Position() const {return ShapesMap::Position{x,y};}
};



auto random_pose(const ShapesMap& map)
{
	static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> d(0, 1);
    int x, y, w, h;
    std::tie(w, h) = map.size();
    std::tie(x, y) = map.bottom_left();
    
    return Pose{ x + d(gen) * w, 
    			 y + d(gen) * h, 
    			 2*Pi*d(gen) };    
}


auto measurement_with_coords(Pose p, ShapesMap& map, int num_rays = 16)
{
	std::vector<int> distances(num_rays, 10000);
	std::vector<ShapesMap::Position> end_points(num_rays);
	auto coord = std::make_tuple(p.x, p.y);
	if ( !map.is_occupied(coord))
	{
		auto da = 2 * Pi / num_rays;
		for (int i = 0; i < num_rays; ++i)
		{
			std::tie(distances[i], end_points[i]) = map.min_distance_towards(coord, p.direction + i*da);		
		}
	}

	return std::make_tuple(distances, end_points);
}


auto measurement(Pose p, ShapesMap& map, int num_rays = 16)
{
	std::vector<int> distances(num_rays, 10000);
	auto coord = std::make_tuple(p.x, p.y);
	if ( !map.is_occupied(coord))
	{
		auto da = 2 * Pi / num_rays;
		for (int i = 0; i < num_rays; ++i)
		{
			std::tie(distances[i], std::ignore) = map.min_distance_towards(coord, p.direction + i*da);		
		}
	}
	return distances;
}


auto generate_scene()
{
	std::vector<std::string> polygons{	
		"POLYGON ((-685 -220,-775 -219,-775 -215,-685 -215,-685 -220))",
		"POLYGON ((-495 -515,-495 -220,-610 -219,-609 -215,-489 -215,-490 -515,-495 -515))",
		"POLYGON ((-780 449,805 450,805 445,-779 445,-780 449))",
		"POLYGON ((-779 445,-775 445,-775 -515,800 -515,800 445,805 445,805 -519,-780 -520,-779 445))",
		"POLYGON ((-295 -219,-490 -219,-490 -215,-295 -215,-295 -219))",
		"POLYGON ((-69 -515,-69 -220,-185 -219,-185 -215,-65 -215,-65 -515,-69 -515))",
		"POLYGON ((420 -515,420 -219,305 -219,305 -215,425 -215,425 -515,420 -515))",
		"POLYGON ((310 445,309 150,425 150,425 145,305 145,305 445,310 445))",
		"POLYGON ((-485 445,-485 149,-370 149,-370 145,-490 145,-489 445,-485 445))",
		"POLYGON ((-65 445,-65 149,50 149,50 145,-69 145,-69 445,-65 445))",
		"POLYGON ((-69 145,-265 145,-265 149,-69 149,-69 145))",
		"POLYGON ((799 145,605 145,605 149,799 149,799 145))",
		"POLYGON ((-685 145,-775 145,-775 149,-685 149,-685 145))",
		"POLYGON ((-490 145,-580 145,-580 149,-490 149,-490 145))",
		"POLYGON ((585 -220,425 -220,425 -215,585 -215,585 -220))",
		"POLYGON ((130 -220,-65 -220,-65 -215,130 -215,130 -220))",
		"POLYGON ((799 -220,709 -220,709 -215,799 -215,799 -220))",
		"POLYGON ((305 145,145 145,145 149,305 149,305 144))"
	};



	std::ifstream file("/home/ppopov/maps/output.csv");
	ShapesMap map;

	for( std::string line; getline( file, line ); )
	{

		auto pos_a = line.find_first_of('\"') + 1;
		auto pos_b = line.find_last_of('\"');
		if ( pos_b > pos_a )
			std::cout << line.substr(pos_a, pos_b - pos_a) << std::endl;
	}


	

	for (auto& s: polygons)
	{
		map.add_wtk(s);
	}

	return map;
}


}