#include <simulations/flatworld.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(flatworld_simulation)

BOOST_AUTO_TEST_CASE( test_sensor_simple )
{
    flatworld::Scene scene;

	{
		flatworld::Polygon poly;
	    boost::geometry::read_wkt(
	        "POLYGON((0.0 0.0, 0.0 10.0, 1.0 10.0, 1.0 0.0))", poly);
	    scene.add(std::move(poly));
	}

	{
		flatworld::Polygon poly;
	    boost::geometry::read_wkt(
	        "POLYGON((9.0 0.0, 9.0 10.0, 10.0 10.0, 10.0 0.0))", poly);
	    scene.add(std::move(poly));
	}

	auto r = scene.measure_from({5.0, 5.0});
	int a = flatworld::Scene::num_rays;

	BOOST_CHECK_EQUAL(r.size(), a);
	BOOST_CHECK_EQUAL(r[0], 4.0);
	BOOST_CHECK_EQUAL(r[2], 500.0);
}

BOOST_AUTO_TEST_SUITE_END()