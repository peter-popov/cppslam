#include <map/shapes_map.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(flatworld_simulation)

BOOST_AUTO_TEST_CASE( test_sensor_simple )
{
	using namespace pfcpp::maps; 
	ShapesMap map;

	map.add_wkt("POLYGON((0.0 0.0, 0.0 10.0, 1.0 10.0, 1.0 0.0))");
	map.add_wkt("POLYGON((9.0 0.0, 9.0 10.0, 10.0 10.0, 10.0 0.0))");

	//
	// Check size
	double widht = 0.0, height = 0.0;
	std::tie(widht, height) = map.size();
	BOOST_CHECK_CLOSE(10.0, widht, 0.00001);
	BOOST_CHECK_CLOSE(10.0, height, 0.00001);

	//
	// Check distance to obstacle
	ShapesMap::coord_t dist = 0;
	ShapesMap::Position pos;
	std::tie(dist, pos) = map.min_distance_towards(std::make_tuple(5.0, 5.0), 0, 100, 0);
	BOOST_CHECK_CLOSE(4.0, dist, 0.00001);

	dist = 0;
	//std::tie(dist, pos) = map.min_distance_towards(std::make_tuple(5.0, 5.0), 1.57, 15, 0);
	//BOOST_CHECK_CLOSE(15.0, dist, 0.00001);
}

BOOST_AUTO_TEST_SUITE_END()