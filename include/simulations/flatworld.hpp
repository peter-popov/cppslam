#pragma once

#include <boost/geometry.hpp>
#include <utility>

namespace flatworld
{

namespace bg = boost::geometry;

typedef double Coord;
typedef bg::model::point<Coord, 2, bg::cs::cartesian> Point;
typedef bg::model::box<Point> Box;
typedef bg::model::polygon<Point> Polygon;
typedef bg::model::linestring<Point> Segment;

auto move_point(const Point& p, const Point& m)
{
	auto res = p;
	bg::add_point(res, m);
	return res;
}

class Scene
{
public:
	static const int num_rays = 4;

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
	    return Point{bg::get<0>(m_bbox.min_corner()) + d(gen)*width(),
	    			 bg::get<1>(m_bbox.min_corner()) + d(gen)*height()};
	}

	bool empty(Point p) const
	{
		for (auto& obj: m_objects)
			if (bg::within(p, obj))
				return false;
		return true;
	}

	std::vector<Coord> measure_from(Point p) const
	{
		std::vector<Coord> res(num_rays);
		auto da = 2*boost::math::constants::pi<double>( ) / num_rays;
		for (int i = 0; i < num_rays; ++i)
		{
			Point touch_point;
			res[i] = calculate_distance(p, 0 + i*da, touch_point);
		}
		return res;
	}

	std::vector<Point> sense_points(Point p) const
	{
		std::vector<Point> res(num_rays, 0);
		if (empty(p))
		{
			auto da = 2*boost::math::constants::pi<double>( ) / num_rays;
			for (int i = 0; i < num_rays; ++i)
				calculate_distance(p, 0.0 + i*da, res[i]);		
		}
		return res;
	}

	double calculate_distance(Point p, double heading, Point& touch_point) const
	{
		static double max_length = 1000;		
		auto end = Point{cos(heading), sin(heading)};
		bg::multiply_value(end, max_length);
		bg::add_point(end, p);

		Segment ray;
		bg::append(ray, p);
		bg::append(ray, end);

		auto min_dist = max_length;
		touch_point = p;
		for (auto& poly: m_objects)
		{
			Segment res;
			bg::intersection(poly, ray, res);
			for (auto& pt: res)
			{
				auto dist = bg::distance(p, pt);
				if (min_dist > dist)
				{
					min_dist = dist;
					touch_point = pt;
				}
			}
					
		}
		return min_dist;
	}


private:
	std::vector<Polygon> m_objects;	 
	Box m_bbox;
};	


auto generate_scene()
{
	std::vector<std::string> polygons{	
		"POLYGON ((647.593220338984111 -220.203389830508286,646.508474576272192 -212.610169491525198,811.389830508475825 -213.694915254237117,808.13559322034007 -220.203389830508286,647.593220338984111 -220.203389830508286))",
		"POLYGON ((-68.33898305084665 -214.779661016948921,79.186440677966971 -214.779661016948921,75.932203389831329 -220.203389830508286,-70.508474576270373 -220.203389830508286,-68.33898305084665 -214.779661016948921))",
		"POLYGON ((377.491525423729854 -504.406779661016856,379.661016949153463 -220.203389830508286,187.661016949153463 -222.372881355932009,188.745762711865268 -215.86440677966084,564.06779661017049 -214.779661016948921,560.813559322034962 -219.118644067796367,385.084745762712828 -221.28813559322009,382.915254237289219 -506.576271186440579,377.491525423729854 -504.406779661016856))",
		"POLYGON ((-491.389830508474176 -220.203389830508286,-492.474576271186038 -215.86440677966084,-342.779661016948694 -216.949152542372758,-342.779661016948694 -221.288135593220204,-491.389830508474176 -220.203389830508286))",
		"POLYGON ((-75.932203389829965 -508.745762711864359,-74.847457627118047 -220.203389830508286,-248.406779661016458 -221.288135593220204,-249.491525423728262 -215.86440677966084,-68.338983050846878 -214.779661016949035,-69.423728813558796 -513.084745762711805,-75.932203389829965 -508.745762711864359))",
		"POLYGON ((-683.932203389830306 -220.745762711864188,-777.220338983050624 -220.203389830508286,-777.220338983050624 -216.406779661016742,-683.932203389830306 -216.406779661016742,-683.932203389830306 -220.745762711864188))",
		"POLYGON ((-493.016949152542111 -511.999999999999886,-494.101694915253972 -220.203389830508286,-607.999999999999773 -221.288135593220147,-606.915254237287854 -216.406779661016742,-489.762711864406526 -216.406779661016742,-489.762711864406526 -513.084745762711691,-493.016949152542111 -511.999999999999886))",
		"POLYGON ((-786.440677966101589 451.796610169491601,803.796610169491487 450.711864406779682,800.542372881355959 442.03389830508479,-788.610169491525312 443.118644067796595,-786.440677966101589 451.796610169491601))",
		"POLYGON ((-788.610169491525312 443.118644067796595,-779.93220338983042 443.118644067796595,-773.423728813559251 -513.627118644067764,805.966101694915324 -503.864406779660953,800.542372881355959 448.542372881355959,807.050847457627242 447.457627118644155,814.644067796610216 -511.457627118644041,-782.101694915254143 -523.389830508474574,-788.610169491525312 443.118644067796595))"
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