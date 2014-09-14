#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <utility>

namespace flatworld
{

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef double Coord;
typedef bg::model::point<Coord, 2, bg::cs::cartesian> Point;
typedef bg::model::box<Point> Box;
typedef bg::model::polygon<Point> Polygon;
typedef bg::model::segment<Point> Segment;
typedef std::pair<Box, size_t> IndexItem;

auto move_point(const Point& p, const Point& m)
{
	auto res = p;
	bg::add_point(res, m);
	return res;
}


struct Pose
{
	double x, y, direction;

	operator Point() const {return {x,y};}
};


class Scene
{
public:
	static const int num_rays = 8;
	mutable size_t num_intersections = 0;

public:

	Scene()
	: m_bbox({0.0, 0.0}, {0.0, 0.0})
	{		
	}

	void add(Polygon&& poly)
	{
		m_objects.push_back(std::forward<Polygon>(poly));
		Box tmp;
		bg::envelope(m_objects.back(), tmp);
		bg::expand(m_bbox, tmp);

		auto ring = bg::exterior_ring(m_objects.back());
		auto next_pos = bg::closing_iterator<decltype(ring)>(ring);

		for(auto& v: ring)
		{
			auto next_v = *++next_pos;			
			m_segments.emplace_back(v, next_v);
			Box sbb;
			bg::envelope(m_segments.back(), sbb);

			rtree.insert(std::make_pair(sbb, m_segments.size()-1));			
		}		
	}	


	auto& objects() const {return m_objects;}

	auto& bbox() const {return m_bbox;}

	auto width() const
	{
		return std::abs(bg::get<0>(m_bbox.max_corner()) - bg::get<0>(m_bbox.min_corner()));
	}

	auto height() const
	{
		return std::abs(bg::get<1>(m_bbox.max_corner()) - bg::get<1>(m_bbox.min_corner()));
	}


	auto random_state() const
	{
		static std::random_device rd;
	    static std::mt19937 gen(rd());
	    std::uniform_real_distribution<> d(0, 1);
	    return Pose{ bg::get<0>(m_bbox.min_corner()) + d(gen)*width(),
	    			 bg::get<1>(m_bbox.min_corner()) + d(gen)*height(),
	    			 2 * 2*boost::math::constants::pi<double>( ) * d(gen)};
	}

	bool empty(Point p) const
	{
		for (auto& obj: m_objects)
			if (bg::within(p, obj))
				return false;
		return true;
	}

	std::vector<Coord> measure_from(Pose p) const
	{
		std::vector<Coord> res(num_rays, 10000);

		if (bg::within(Point{p.x, p.y}, m_bbox))
		{
			auto da = 2*boost::math::constants::pi<double>( ) / num_rays;
			for (int i = 0; i < num_rays; ++i)
			{
				Point touch_point;
				res[i] = calculate_distance({p.x, p.y}, p.direction + i*da, touch_point);
			}
		}
		return res;
	}

	std::vector<Point> sense_points(Pose p) const
	{
		std::vector<Point> res(num_rays, 0);
		if (empty({p.x, p.y}))
		{
			auto da = 2*boost::math::constants::pi<double>( ) / num_rays;
			for (int i = 0; i < num_rays; ++i)
				calculate_distance({p.x, p.y}, p.direction + i*da, res[i]);		
		}
		return res;
	}

	auto make_box(Point a, Point b) const
	{
		return Box({std::min(bg::get<0>(a), bg::get<0>(b)), std::min(bg::get<1>(a), bg::get<1>(b))} , 
				{std::max(bg::get<0>(a), bg::get<0>(b)), std::max(bg::get<1>(a), bg::get<1>(b))});
	}

	double calculate_distance(Point p, double heading, Point& touch_point) const
	{
		static double max_range = 1000;		
		auto end = Point{cos(heading), sin(heading)};
		bg::multiply_value(end, max_range);
		bg::add_point(end, p);

		Segment ray{p, end};

		query_result.resize(0);
		rtree.query(bgi::intersects( make_box(p, end) ), std::back_inserter(query_result));
		
		auto min_dist = max_range;
		touch_point = end;
		intersection_result.resize(0);
		for (auto& idx: query_result)
		{
			auto& segm = m_segments[idx.second];			
			bg::intersection(segm, ray, intersection_result);
			num_intersections++;					
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

		return min_dist;
	}


private:
	std::vector<Polygon> m_objects;	 
	std::vector<Segment> m_segments;
	bgi::rtree< IndexItem, bgi::quadratic<4> > rtree;
	mutable std::vector<IndexItem> query_result;
	mutable std::vector<Point> intersection_result;
	Box m_bbox;
};	


auto generate_scene()
{
	std::vector<std::string> polygons{	
		"POLYGON ((-684.881355932203178 -220.067796610169268,-774.983050847457321 -219.999999999999801,-774.983050847457321 -215.050847457626872,-684.949152542372758 -215.186440677965862,-684.881355932203178 -220.067796610169268))",
		"POLYGON ((-494.949152542372644 -514.983050847457662,-494.915254237287854 -220.033898305084563,-610.033898305084676 -219.932203389830278,-609.966101694914983 -215.118644067796367,-489.999999999999773 -215.050847457626872,-490.067796610169239 -514.983050847457548,-494.949152542372644 -514.983050847457662))",
		"POLYGON ((-780.067796610169466 449.966101694915324,804.974576271186379 450.0,804.983050847457662 445.025423728813621,-779.999999999999886 445.084745762711862,-780.067796610169466 449.966101694915324))",
		"POLYGON ((-779.93220338983042 445.084745762711805,-774.847457627118501 445.016949152542338,-775.008474576271055 -515.000000000000114,800.0 -514.983050847457889,800.016949152542452 445.008474576271169,804.991525423728945 444.991525423728888,805.016949152542338 -519.864406779661181,-780.016949152542225 -520.016949152542338,-779.93220338983042 445.084745762711805))",
		"POLYGON ((-295.033898305071489 -219.999999999999773,-490.016949152533186 -219.966101694915068,-490.016949152533186 -215.016949152542139,-295.033898305071546 -215.050847457626872,-295.033898305071489 -219.999999999999773))",
		"POLYGON ((-69.999999999983032 -514.983050847457662,-69.966101694898242 -220.033898305084563,-185.084745762695064 -219.932203389830278,-185.01694915252537 -215.118644067796367,-65.050847457610161 -215.050847457626872,-65.118644067779627 -514.983050847457548,-69.999999999983032 -514.983050847457662))",
		"POLYGON ((420.033898305122761 -514.847457627118729,420.067796610207552 -219.898305084745573,304.949152542410729 -219.796610169491288,305.016949152580423 -214.983050847457378,424.983050847495633 -214.915254237287883,424.915254237326167 -514.847457627118501,420.033898305122761 -514.847457627118729))",
		"POLYGON ((310.007045316499728 445.200920582057847,309.868937329469475 150.251798425806754,424.987538281844252 150.109430480803695,424.91804097769483 145.295895412705988,304.951922817017874 145.270484517823405,305.125689688968123 445.202645233764883,310.007045316499728 445.200920582057847))",
		"POLYGON ((-485.031685863860957 444.853742096826238,-485.16979385089121 149.904619940575145,-370.051192898516433 149.762251995572086,-370.120690202665855 144.948716927474379,-490.086808363342811 144.923306032591796,-489.913041491392562 444.855466748533274,-485.031685863860957 444.853742096826238))",
		"POLYGON ((-64.251361763163516 444.853742096826238,-64.389469750193769 149.904619940575145,50.729131202181009 149.762251995572086,50.659633898031586 144.948716927474379,-69.30648426264537 144.923306032591796,-69.13271739069512 444.855466748533274,-64.251361763163516 444.853742096826238))",
		"POLYGON ((-69.541472147148227 144.884587978410252,-264.524522994609924 144.918486283494957,-264.524522994609924 149.867638825867886,-69.541472147148284 149.833740520783152,-69.541472147148227 144.884587978410252))",
		"POLYGON ((799.446276387542866 144.5374094931787,604.463225540081226 144.571307798263405,604.463225540081226 149.520460340636333,799.446276387542866 149.4865620355516,799.446276387542866 144.5374094931787))",
		"POLYGON ((-684.881355932203064 144.469612883009205,-774.983050847457207 144.537409493178671,-774.983050847457207 149.4865620355516,-684.949152542372644 149.35096881521261,-684.881355932203064 144.469612883009205))",
		"POLYGON ((-490.461404202507879 144.469612883009205,-580.563099117762022 144.537409493178671,-580.563099117762022 149.4865620355516,-490.529200812677459 149.35096881521261,-490.461404202507879 144.469612883009205))",
		"POLYGON ((585.237150999646701 -220.173589242615577,424.624770190113225 -220.139690937530872,424.624770190113225 -215.190538395157944,585.237150999646587 -215.224436700242677,585.237150999646701 -220.173589242615577))",
		"POLYGON ((130.08615686102101 -220.347178485231382,-64.896893986440688 -220.313280180146677,-64.896893986440688 -215.364127637773748,130.086156861020953 -215.398025942858482,130.08615686102101 -220.347178485231382))",
		"POLYGON ((799.653846918112322 -220.414975095400791,709.552152002858179 -220.347178485231325,709.552152002858179 -215.398025942858396,799.586050307942742 -215.533619163197386,799.653846918112322 -220.414975095400791))",
		"POLYGON ((304.716934932514789 144.710998735794931,144.104554122981313 144.744897040879636,144.104554122981313 149.694049583252564,304.716934932514675 149.660151278167831,304.716934932514789 144.710998735794931))"
	};


	Scene scene;

	for (auto& s: polygons)
	{
		Polygon poly;
	    boost::geometry::read_wkt(s, poly);
	    scene.add(std::move(poly));
	}

	return scene;
}


}