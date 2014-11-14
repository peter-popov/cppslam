#include <pfcpp/sensor.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

//=================================================================================================
// Unit-test for beam sensor model
//=================================================================================================
BOOST_AUTO_TEST_SUITE(beam_sensor)

//
//
BOOST_AUTO_TEST_CASE(test_sensor_model_returns_probability) {

    auto setting = pfcpp::SensorSetings{100, {1.0, 0.0, 0.0, 0.0}, 50, 0.0};

    pfcpp::BeamSensorModel model(setting);

    BOOST_CHECK_GE(model(30.0, 30.0), 0.0);
    BOOST_CHECK_GE(model(35.0, 30.0), 0.0);
    BOOST_CHECK_GE(model(0.0, 30.0), 0.0);
    BOOST_CHECK_GE(model(30.0, 0.0), 0.0);
    BOOST_CHECK_GE(model(95.0, 20.0), 0.0);

    BOOST_CHECK_LE(model(30.0, 30.0), 1.0);
    BOOST_CHECK_LE(model(35.0, 30.0), 1.0);
    BOOST_CHECK_LE(model(0.0, 30.0), 1.0);
    BOOST_CHECK_LE(model(30.0, 0.0), 1.0);
    BOOST_CHECK_LE(model(95.0, 20.0), 1.0);
}

//
//
BOOST_AUTO_TEST_CASE(test_sensor_model_max_value_when_expected_value_matches_measured) {

    auto setting = pfcpp::SensorSetings{100, {1.0, 0.0, 0.0, 0.0}, 50, 0.0};

    pfcpp::BeamSensorModel model(setting);

    BOOST_CHECK_CLOSE(model(30.0, 30.0), model(50.0, 50.0), 0.005);
    BOOST_CHECK_LE(model(25.0, 30.0), model(30.0, 30.0));
    BOOST_CHECK_LE(model(35.0, 30.0), model(30.0, 30.0));

    BOOST_CHECK_LE(model(0.0, 30.0), model(30.0, 30.0));
    BOOST_CHECK_LE(model(95.0, 30.0), model(30.0, 30.0));
}

BOOST_AUTO_TEST_SUITE_END()