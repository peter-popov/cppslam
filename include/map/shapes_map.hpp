#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <utility>
#include <istream>
#include <tuple>

namespace pfcpp
{
namespace maps
{



class ShapesMap
{
public:

	typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point;
	//typedef std::tuple<double, double> Point;
	typedef boost::geometry::model::box<Point> Box;
	typedef boost::geometry::model::polygon<Point> Polygon;
	typedef boost::geometry::model::segment<Point> Segment;

public:

	typedef std::tuple<double, double> Position;
	
	ShapesMap read_from_wtk(std::istream& is);

public:
	ShapesMap();

	void add(Polygon&& poly);

	void add_wtk(std::string feature);

	Position bottom_left() const;

	Position top_right() const;

	std::tuple<double, double> size() const;

	bool is_occupied(Position point) const;

	std::tuple<double, Position> min_distance_towards(Position p, double heading);


	std::vector<Polygon> objects;
private:		 
	std::vector<Segment> segments;
	typedef std::pair<Box, size_t> IndexItem;
	boost::geometry::index::rtree< IndexItem, boost::geometry::index::quadratic<4> > rtree;
	mutable std::vector<IndexItem> query_result;
	mutable std::vector<Point> intersection_result;
	Box bbox;
};

} //maps
} //pfcpp