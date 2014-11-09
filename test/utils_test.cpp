#include <pfcpp/utils.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(utils)

struct P
{
	int id;
	double weight;
};


BOOST_AUTO_TEST_CASE( normilize_test )
{
	std::vector<P> particles{ {0, 0.4}, {1, 0.4}, {2, 0.8} };    		
	pfcpp::normalize(std::begin(particles), std::end(particles));
	BOOST_CHECK_EQUAL(0, particles[0].id);
	BOOST_CHECK_CLOSE(0.25, particles[0].weight, 0.00001);

	BOOST_CHECK_EQUAL(1, particles[1].id);
	BOOST_CHECK_CLOSE(0.25, particles[1].weight, 0.00001);

	BOOST_CHECK_EQUAL(2, particles[2].id);
	BOOST_CHECK_CLOSE(0.5, particles[2].weight, 0.00001);
}


BOOST_AUTO_TEST_CASE( multimodal_distr_draw_test )
{
	std::vector<P> particles{ {0, 0.4}, {1, 0.4}, {2, 0.8} };    		
	
	pfcpp::MultimodalDistribution<double> mmdistr(std::begin(particles),
		std::end(particles), [](auto x){ return x.weight; });

	BOOST_CHECK_EQUAL(0, particles[mmdistr.draw(0.1)].id);
	BOOST_CHECK_EQUAL(0, particles[mmdistr.draw(0.2)].id);
	BOOST_CHECK_EQUAL(0, particles[mmdistr.draw(0)].id);
	BOOST_CHECK_EQUAL(0, particles[mmdistr.draw(0.3999999)].id);

	BOOST_CHECK_EQUAL(1, particles[mmdistr.draw(0.40001)].id);
	BOOST_CHECK_EQUAL(1, particles[mmdistr.draw(0.7)].id);


	BOOST_CHECK_EQUAL(2, particles[mmdistr.draw(0.800001)].id);
	BOOST_CHECK_EQUAL(2, particles[mmdistr.draw(0.9)].id);
	BOOST_CHECK_EQUAL(2, particles[mmdistr.draw(18)].id);
}

BOOST_AUTO_TEST_CASE( multimodal_distr_draw_next_test )
{
	std::vector<P> particles{ {0, 0.4}, {1, 0.4}, {2, 0.8} };    		
	
	pfcpp::MultimodalDistribution<double> mmdistr(std::begin(particles),
		std::end(particles), [](auto x){ return x.weight; });

	BOOST_CHECK_EQUAL(0, particles[mmdistr.draw_next(0.1)].id);
	BOOST_CHECK_EQUAL(0, particles[mmdistr.draw_next(0.3999)].id);
	BOOST_CHECK_EQUAL(1, particles[mmdistr.draw_next(0.40001)].id);
	BOOST_CHECK_EQUAL(1, particles[mmdistr.draw_next(0.0001)].id);
	BOOST_CHECK_EQUAL(2, particles[mmdistr.draw_next(0.8001)].id);
	BOOST_CHECK_EQUAL(2, particles[mmdistr.draw_next(0.1)].id);
}



BOOST_AUTO_TEST_SUITE_END()