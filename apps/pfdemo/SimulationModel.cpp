#include "SimulationModel.hpp"

#include <map/shapes_map.hpp>
#include <pfcpp/mcl.hpp>
#include <simulations/flatworld.hpp>
#include <pfcpp/sensor.hpp>

using namespace pfcpp::maps;
using namespace flatworld;



struct SimulationModel::Impl
{
	size_t step = 0;

	pfcpp::maps::ShapesMap map;

    flatworld::Pose robot_pos;
    pfcpp::ParticleFilter<flatworld::Pose> pf;

    std::vector<Control> path;
};




SimulationModel::SimulationModel( SimulationConfig config, std::tuple<double, double, double> start_pos, std::vector<Control> path, std::string map_url )
:pimpl(new Impl)
{
	pimpl->map = flatworld::generate_scene();
	pimpl->path = path;
	std::tie(pimpl->robot_pos.x,
			 pimpl->robot_pos.y,
			 pimpl->robot_pos.direction) = start_pos;
    
    std::vector<Pose> states;
    for (int i = 0; i < config.particle_count; ++i)
    {
        auto p = random_pose(pimpl->map);
        if (pimpl->map.is_occupied(std::make_tuple(p.x,p.y)))
            continue;
        states.push_back(p);
    }

    pimpl->pf = pfcpp::ParticleFilter<flatworld::Pose>(states, {{0.08,0.5,0.005,0.1,0.0,0.0}});	
}

SimulationModel::~SimulationModel()
{}

std::tuple<QPointF, double> SimulationModel::current_position() const
{
	return std::make_tuple(QPointF{pimpl->robot_pos.x, pimpl->robot_pos.y}, pimpl->robot_pos.direction); 
}

std::vector<QPointF> SimulationModel::measurment_end_points() const
{
	std::vector<ShapesMap::Position> sense_points;
    std::tie(std::ignore, sense_points) = measurement_with_coords(pimpl->robot_pos, pimpl->map);
 	std::vector<QPointF> res;
 	for (auto p: sense_points)
 		res.emplace_back(std::get<0>(p), std::get<1>(p));
 	return res;   
}

size_t SimulationModel::particle_count( ) const
{
	return pimpl->pf.particles.size();
}

std::tuple<QPointF, double> SimulationModel::particle(size_t i) const
{
	auto p = pimpl->pf.particles[i].state;
	return std::make_tuple(QPointF{p.x, p.y}, p.direction);
}


void SimulationModel::update()
{
	pfcpp::BeamSensorModel sensor_model;
	pfcpp::VelocityMotionModelSampler movement;
	pimpl->robot_pos = movement(pimpl->robot_pos, pimpl->path[pimpl->step]);

    pimpl->pf([&](auto p){ return sensor_model(flatworld::measurement(pimpl->robot_pos, pimpl->map, 16),
                                     		   flatworld::measurement(p, pimpl->map, 16));},
    		   pimpl->path[pimpl->step]);

    pimpl->step++;		
}

QPoint to_q_point(const ShapesMap::Point& x)
{
      return {boost::geometry::get<0>(x), boost::geometry::get<1>(x)};
}


std::vector<QPolygonF> SimulationModel::export_map()
{
	std::vector<QPolygonF> res;
	for (auto& poly: pimpl->map.objects)
	{
		QPolygonF qpoly;          
		boost::geometry::for_each_point(poly,[&](auto x){qpoly.push_back(to_q_point(x));});
		res.push_back(qpoly);
	}
	return res;
}