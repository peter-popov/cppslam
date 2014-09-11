#define BOOST_TEST_MODULE "resample tests"
#define BOOST_TEST_DYN_LINK
#include <pfcpp/resample.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_resample_weight_member )
{
	struct P
	{
		int id;
		double weight;
	};

	std::vector<P> particles{ {0, 0.1}, {1, 0.2}, {2, 0.7} };    	

	auto resampled = mcl::resample(particles);
}


struct MyP
{
	int id;
	double other_field;
};

auto get_weight(const MyP& p) {return p.other_field;};

BOOST_AUTO_TEST_CASE( test_resample_custom )
{
	std::vector<MyP> particles{ {0, 0.1}, {1, 0.2}, {2, 0.7} };    	

	auto resampled = mcl::resample(particles);
}