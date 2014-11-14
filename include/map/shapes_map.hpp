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
	typedef double coord_t;

	typedef boost::geometry::model::point<coord_t, 2, boost::geometry::cs::cartesian> Point;
	typedef boost::geometry::model::box<Point> Box;
	typedef boost::geometry::model::polygon<Point> Polygon;

public:

	typedef std::tuple<coord_t, coord_t> Position;
	typedef std::pair<Position, Position> Segment;

public:
	ShapesMap();

	void add(Polygon&& poly);

	void add_wkt(std::string feature);

	Position bottom_left() const;

	Position top_right() const;

	std::tuple<coord_t, coord_t> size() const;

	bool is_occupied(Position point) const;

	std::tuple<coord_t, Position> min_distance_towards(Position p, double heading, coord_t max_range, coord_t noise = 0);


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