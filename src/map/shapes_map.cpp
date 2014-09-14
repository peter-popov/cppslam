#include <map/shapes_map.hpp>

namespace pfcpp
{
namespace maps
{

namespace bg = boost::geometry;


auto tuplify(ShapesMap::Point p)
{
	return std::make_tuple(bg::get<0>(p), bg::get<1>(p));
}

template<typename T>
ShapesMap::Point untuplify(T p)
{
	return {std::get<0>(p), std::get<1>(p)};
}



ShapesMap::ShapesMap()
: bbox({0.0, 0.0}, {0.0, 0.0})
{	
}


void ShapesMap::add(Polygon&& poly)
{
	objects.push_back(std::forward<Polygon>(poly));
	Box tmp;
	bg::envelope(objects.back(), tmp);
	bg::expand(bbox, tmp);

	auto ring = bg::exterior_ring(objects.back());
	auto next_pos = bg::closing_iterator<decltype(ring)>(ring);

	for(auto& v: ring)
	{
		auto next_v = *++next_pos;			
		segments.emplace_back(v, next_v);
		Box sbb;
		bg::envelope(segments.back(), sbb);

		rtree.insert(std::make_pair(sbb, segments.size()-1));			
	}	
}

void ShapesMap::add_wtk(std::string feature)
{
	Polygon poly;
	bg::read_wkt(feature, poly);
    add(std::move(poly));
}

auto ShapesMap::bottom_left() const -> Position
{
	return tuplify(bbox.min_corner());
}

auto ShapesMap::top_right() const -> Position
{
	return tuplify(bbox.max_corner());
}

std::tuple<double, double> ShapesMap::size() const
{
	return std::make_tuple(bg::get<0>(bbox.max_corner()) - bg::get<0>(bbox.min_corner()),
						   bg::get<1>(bbox.max_corner()) - bg::get<1>(bbox.min_corner()));
}

bool ShapesMap::is_occupied(Position point) const
{
	query_result.resize(0);
	rtree.query(bg::index::intersects(untuplify(point)), std::back_inserter(query_result));
	return !query_result.empty();
}

template<typename Point>
auto make_box(Point a, Point b)
{
	return ShapesMap::Box({std::min(bg::get<0>(a), bg::get<0>(b)), std::min(bg::get<1>(a), bg::get<1>(b))} , 
			   			  {std::max(bg::get<0>(a), bg::get<0>(b)), std::max(bg::get<1>(a), bg::get<1>(b))});
}

auto ShapesMap::min_distance_towards(Position pos, double heading) -> std::tuple<double, Position>
{
	static double maximum_range = 1000;		
	auto p = untuplify(pos);
	auto end = Point{cos(heading), sin(heading)};
	bg::multiply_value(end, maximum_range);
	bg::add_point(end, p);

	Segment ray{p, end};

	query_result.resize(0);
	rtree.query(bg::index::intersects( make_box(p, end) ), std::back_inserter(query_result));
	
	auto min_dist = maximum_range;
	auto touch_point = end;
	intersection_result.resize(0);
	for (auto& idx: query_result)
	{
		auto& segm = segments[idx.second];			
		bg::intersection(segm, ray, intersection_result);
	}

	for (auto& pt: intersection_result)
	{
		auto dist = bg::distance(p, pt);
		if (min_dist > dist)
		{
			min_dist = dist;
			touch_point = pt;
		}
	}

	return std::make_tuple(min_dist, tuplify(touch_point));
}

}
}