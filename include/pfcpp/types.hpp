#pragma once

namespace pfcpp
{

//-------------------------------------------------------------------------------------------------
// Position concept
//-------------------------------------------------------------------------------------------------
template<typename Position>
auto get_x(const Position& p) -> decltype(Position::x) {
	return p.x;
}

template<typename Position>
auto get_y(const Position& p) -> decltype(Position::y) {
	return p.y;
}

template<typename Position>
auto get_heading(const Position& p) -> decltype(Position::heading) {
	return p.heading;
}


//-------------------------------------------------------------------------------------------------
// Particle and ustils
//-------------------------------------------------------------------------------------------------
/**
 * Particle
 */
template<typename State, typename weight_t = double>
struct Particle {
	State state;
	weight_t weight;
};

template<typename State>
auto get_x(const Particle<State>& p) {
	return get_x(p.state);
}

template<typename State>
auto get_y(const Particle<State>& p) {
	return get_y(p.state);
}

template<typename State>
auto get_heading(const Particle<State>& p) {
	return get_heading(p.state);
}


} //pfcpp