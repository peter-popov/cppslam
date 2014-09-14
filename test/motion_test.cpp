#include <pfcpp/motion.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(motion)

struct State
{
	int x;
	int y;
	double direction;
};

struct Control
{
	int v;
	double w;
};

BOOST_AUTO_TEST_CASE( test_velocity_motion_model )
{
	std::vector<std::string> pic;
	for (size_t i = 0; i < 30; ++i)
	{
		pic.push_back(std::string(30,' '));
	}

	mcl::VelocityMotionModelSampler ms({0.001,0.5,0.02,0.5,0.1,0.1});

	auto start_pose = State{10, 0, 0.785398163};
	auto control = Control{20, 0.785398163};

	for (size_t i = 0; i < 100; ++i)
	{
		State sn = ms(start_pose, control);
		if ( sn.y < 0 || sn.x < 0 || sn.y >= 30 || sn.x >= 30 )
			std::cout << "over:" << sn.x << "; " << sn.y << std::endl;
		else
			pic[sn.y][sn.x] = '*';
	}

	for (auto& s: pic)
	{
		std::cout << s << std::endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()