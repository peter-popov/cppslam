#include <pfcpp/motion.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_SUITE(motion)

struct State {
    double x;
    double y;
    double heading;
};

struct Control {
    double v;
    double w;
};

double get_time(const Control &) { return 1.0; }

BOOST_AUTO_TEST_CASE(test_velocity_motion_model_no_turn) {
    pfcpp::VelocityMotionModelSampler model;

    auto start_pose = State{10, 0, 0.785398163};
    auto control = Control{20, 0.0};

    auto end_pose = model(start_pose, control);

    BOOST_CHECK_CLOSE(start_pose.x + 20 / std::sqrt(2), end_pose.x, 0.0001);
    BOOST_CHECK_CLOSE(start_pose.y + 20 / std::sqrt(2), end_pose.y, 0.0001);
    BOOST_CHECK_CLOSE(start_pose.heading, end_pose.heading, 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()