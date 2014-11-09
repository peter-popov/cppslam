#include <pfcpp/resample.hpp>
#include <boost/test/unit_test.hpp>

//
// Mock random number generator
template<typename T>
struct MockGenerator {
	MockGenerator(std::initializer_list<T> v)
	: numbers(v) {
	}

	T operator()() {
		return numbers[i++ % numbers.size()];
	}

	size_t i = 0;
	std::vector<T> numbers;
};


struct TestParticle {
	int id;
	double weight;
};

struct CustomTestParticle {
	int id;
	double other_field;
};

auto get_weight(const CustomTestParticle& p) {return p.other_field;};


//=================================================================================================
// Unit-test for resampling functionality
//=================================================================================================
BOOST_AUTO_TEST_SUITE(resampling)

//
//
BOOST_AUTO_TEST_CASE( test_resample_compiles_with_custom_particle_class ) {
	std::vector<CustomTestParticle> particles{ {0, 0.1}, {1, 0.2}, {2, 0.7} };    	
	std::vector<CustomTestParticle> resampled;
	pfcpp::multimodal_resample(particles, resampled);	
}

//
//
BOOST_AUTO_TEST_CASE( test_mock_generator ) {
	auto gen = MockGenerator<double>{0.1, 0.2, 0.3};

	BOOST_CHECK_CLOSE(0.1, gen(), 0.00001);
	BOOST_CHECK_CLOSE(0.2, gen(), 0.00001);
	BOOST_CHECK_CLOSE(0.3, gen(), 0.00001);
	BOOST_CHECK_CLOSE(0.1, gen(), 0.00001);
}

//
//
BOOST_AUTO_TEST_CASE( test_resample_multimodal ) {
	auto gen = MockGenerator<double>{0.1, 0.1, 0.9};

	std::vector<TestParticle> particles{ {0, 0.2}, {1, 0.2}, {2, 0.6} };    	
	std::vector<TestParticle> resampled;
	pfcpp::multimodal_resample(particles, resampled, gen);

	BOOST_CHECK_EQUAL(0, resampled[0].id);
	BOOST_CHECK_EQUAL(0, resampled[1].id);
	BOOST_CHECK_EQUAL(2, resampled[2].id);
}

//
//
BOOST_AUTO_TEST_CASE( test_resample_stratified ) {
	auto gen = MockGenerator<double>{0.1, 0.1, 0.9};


	std::vector<TestParticle> particles{ {0, 0.2}, {1, 0.6}, {2, 0.2} };    	
	std::vector<TestParticle> resampled;
	pfcpp::stratified_resample(particles, resampled, gen);

	BOOST_CHECK_EQUAL(0, resampled[0].id);
	BOOST_CHECK_EQUAL(1, resampled[1].id);
	BOOST_CHECK_EQUAL(2, resampled[2].id);
}

//
//
BOOST_AUTO_TEST_CASE( test_resample_systematic ) {
	auto gen = MockGenerator<double>{0.1, 0.1, 0.9};


	std::vector<TestParticle> particles{ {0, 0.2}, {1, 0.6}, {2, 0.2} };    	
	std::vector<TestParticle> resampled;
	pfcpp::systematic_resample(particles, resampled, gen);

	BOOST_CHECK_EQUAL(0, resampled[0].id);
	BOOST_CHECK_EQUAL(1, resampled[1].id);
	BOOST_CHECK_EQUAL(1, resampled[2].id);
}


BOOST_AUTO_TEST_SUITE_END()