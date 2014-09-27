#include <map/shapes_map.hpp>

namespace pfcpp
{
namespace maps
{

namespace bg = boost::geometry;


template<typename Point>
auto make_box(Point a, Point b)
{
	return ShapesMap::Box({std::min(bg::get<0>(a), bg::get<0>(b)), std::min(bg::get<1>(a), bg::get<1>(b))} , 
			   			  {std::max(bg::get<0>(a), bg::get<0>(b)), std::max(bg::get<1>(a), bg::get<1>(b))});
}


template<typename P, typename T>
auto fast_intersect(const P& p1, const P& p2, const P& p3, const P& p4, P& res, T& dist)
{
	T x,y;
	T x1,y1,x2,y2,x3,y3,x4,y4;
	std::tie(x1, y1) = p1;
	std::tie(x2, y2) = p2;
	std::tie(x3, y3) = p3;
	std::tie(x4, y4) = p4;

	auto same_sign = [](auto x, auto y) { return std::signbit(x) == std::signbit(y); };

	T x1lo,x1hi,y1lo,y1hi;

	auto Ax = x2 - x1;
	auto Bx = x3 - x4;

	if (Ax<0) {						/* X bound box test*/
  		x1lo=x2; x1hi=x1;
  	} else {
  		x1hi=x2; x1lo=x1;
  	}
	if(Bx>0) {
  		if(x1hi < x4 || x3 < x1lo) return false;
  	} else {
  		if(x1hi < x3 || x4 < x1lo) return false;
  	}

	auto Ay = y2-y1;
	auto By = y3-y4;

	if(Ay<0) {						/* Y bound box test*/
	  y1lo=y2; y1hi=y1;
	} else {
	  y1hi=y2; y1lo=y1;
	}
	if(By>0) {
	  if(y1hi < y4 || y3 < y1lo) return false;
	} else {
	  if(y1hi < y3 || y4 < y1lo) return false;
	}


	auto Cx = x1-x3;
	auto Cy = y1-y3;
	auto d = By*Cx - Bx*Cy;					/* alpha numerator*/
	auto f = Ay*Bx - Ax*By;					/* both denominator*/
	if(f>0) {						/* alpha tests*/
	  if(d<0 || d>f) return false;
	} else {
	  if(d>0 || d<f) return false;
	}

	auto e = Ax*Cy - Ay*Cx;					/* beta numerator*/
	if(f>0) {						/* beta tests*/
  		if(e<0 || e>f) return false;
  	} else {
  		if(e>0 || e<f) return false;
  	}

/*compute intersection coordinates*/

	if(f==0) return false;
	auto num = d*Ax;						/* numerator */
	auto offset = same_sign(num, f) ? f/2 : -f/2;		/* round direction*/
	x = x1 + (num+offset) / f;				/* intersection x */

	num = d*Ay;
	offset = same_sign(num, f) ? f/2 : -f/2;
	y = y1 + (num+offset) / f;				/* intersection y */

	res = std::make_tuple(x, y);
	dist = std::abs(d * 1000 / f); 

	return true;
}


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
		segments.emplace_back(tuplify(v), tuplify(next_v));		
		rtree.insert(std::make_pair(make_box(v, next_v), segments.size()-1));			
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

auto ShapesMap::size() const -> std::tuple<coord_t, coord_t>
{
	return std::make_tuple(bg::get<0>(bbox.max_corner()) - bg::get<0>(bbox.min_corner()),
						   bg::get<1>(bbox.max_corner()) - bg::get<1>(bbox.min_corner()));
}

bool ShapesMap::is_occupied(Position point) const
{
	auto up = untuplify(point);
	if (!bg::within(up, bbox)) return true;
	for (auto& poly: objects)
	{
		if (bg::within(up, poly)) return true;
	}
	return false;
}



auto ShapesMap::min_distance_towards(Position p, double heading, coord_t maximum_range) -> std::tuple<coord_t, Position>
{
	auto end = Position{std::get<0>(p) + cos(heading)*maximum_range, 
						std::get<1>(p) + sin(heading)*maximum_range};	

	query_result.resize(0);
	rtree.query(bg::index::intersects(make_box(untuplify(p), untuplify(end))), std::back_inserter(query_result));
	
	auto min_dist = maximum_range;
	auto touch_point = end;
	intersection_result.resize(0);
	ShapesMap::Position tmp;
	for (auto& idx: query_result)
	{
		auto& segm = segments[idx.second];			

		auto dist = maximum_range;
		if (fast_intersect(p, end, segm.first, segm.second, tmp, dist))
		{
			if ( dist < min_dist )
			{
				min_dist = dist;
				touch_point = tmp;
			}
		}
	}	

	return std::make_tuple(min_dist, touch_point);
}

}
}