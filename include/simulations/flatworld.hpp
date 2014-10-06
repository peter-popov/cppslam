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


auto measurement_with_coords(Pose p, ShapesMap& map, int num_rays = 16, double max_range = 1000.0)
{
	std::vector<double> distances(num_rays, max_range);
	std::vector<ShapesMap::Position> end_points(num_rays);
	auto coord = std::make_tuple(p.x, p.y);
	if ( !map.is_occupied(coord))
	{
		auto da = 2 * Pi / num_rays;
		for (int i = 0; i < num_rays; ++i)
		{
			std::tie(distances[i], end_points[i]) = map.min_distance_towards(coord, p.direction + i*da, max_range);		
		}
	}

	return std::make_tuple(distances, end_points);
}


auto measurement(Pose p, ShapesMap& map, int num_rays = 16, double max_range = 1000.0)
{
	std::vector<double> distances(num_rays, max_range);
	auto coord = std::make_tuple(p.x, p.y);
	if ( !map.is_occupied(coord))
	{
		auto da = 2 * Pi / num_rays;
		for (int i = 0; i < num_rays; ++i)
		{
			std::tie(distances[i], std::ignore) = map.min_distance_towards(coord, p.direction + i*da, max_range);		
		}
	}
	return distances;
}


auto load_scene(std::string wkt_csv_path)
{
	std::ifstream file(wkt_csv_path);
	ShapesMap map;
	int skip = 1;
	for( std::string line; getline( file, line ); )
	{
		if (skip) {skip--;continue;}
		auto pos_a = line.find_first_of('\"') + 1;
		auto pos_b = line.find_last_of('\"');
		if ( pos_b > pos_a )
			map.add_wtk(line.substr(pos_a, pos_b - pos_a));
	}
	
	return map;
}

}