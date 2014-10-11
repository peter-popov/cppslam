#include "SimulationModel.hpp"

#include <map/shapes_map.hpp>
#include <pfcpp/mcl.hpp>
#include <simulations/flatworld.hpp>
#include <pfcpp/sensor.hpp>

using namespace pfcpp::maps;


struct Simulation::Impl
{
	size_t step = 0;
	pfcpp::maps::ShapesMap map;
    flatworld::Pose robot_pos;
    pfcpp::ParticleFilter<flatworld::Pose> pf;
};

Pose toQPose(flatworld::Pose p)
{
	Pose qp;
	qp.setPosition({p.x, p.y});
	qp.setOrientation(p.direction);
	return qp;
}


template<typename T>
static T* list_at(QQmlListProperty<T> *property, int index)
{
	auto lst = (QList<std::shared_ptr<T>>*)property->data;
	return lst->at(index).get();
}

template<typename T>
static int list_count(QQmlListProperty<T> *property)
{
	auto lst = (QList<std::shared_ptr<Particle>>*)property->data;
	return lst->size();	
}

Simulation::Simulation()
{

}

Simulation::~Simulation()
{
}
	

QQmlListProperty<Particle> Simulation::particles()
{
	return QQmlListProperty<Particle>(this, &m_particles, list_count<Particle>, list_at<Particle>);
}

QQmlListProperty<QPointF> Simulation::sensorBeams()
{
	return QQmlListProperty<QPointF>(this, &m_sensorBeams, list_count<QPointF>, list_at<QPointF>);
}

void Simulation::init(QString mapUrl, Pose* pose)
{
	auto startPosition = *pose;
	pimpl.reset(new Impl);

	pimpl->map = flatworld::load_scene(mapUrl.toStdString());
	pimpl->robot_pos.x = startPosition.position().x();
	pimpl->robot_pos.y = startPosition.position().y();
	pimpl->robot_pos.direction = startPosition.orientation();
    
    std::vector<flatworld::Pose> states;
    for (int i = 0; i < m_mcl->numberOfParticles(); ++i)
    {
        auto p = flatworld::random_pose(pimpl->map);
        if (pimpl->map.is_occupied(std::make_tuple(p.x,p.y)))
            continue;
        states.push_back(p);

    }

    pimpl->pf = pfcpp::ParticleFilter<flatworld::Pose>(states, {{0.4,0.5,0.05,0.1,0.0,0.0}});

    updateParticlesList();
    emit initialized();
}

void Simulation::move(const VelocityControl& control)
{
	
}


void Simulation::updateParticlesList()
{
	m_particles.clear();
	for (auto& particle: pimpl->pf.particles)
	{
		m_particles.push_back(std::make_shared<Particle>());		
		m_particles.back()->setpose(toQPose(particle.state));
		m_particles.back()->setweight(particle.weight);
	}
}




/*
struct SimulationModel::Impl
{
	size_t step = 0;

	pfcpp::maps::ShapesMap map;

    flatworld::Pose robot_pos;
    pfcpp::ParticleFilter<flatworld::Pose> pf;

    std::vector<Control> path;
};




SimulationModel::SimulationModel( SimulationConfig cfg, std::tuple<double, double, double> start_pos, std::vector<Control> path, std::string map_url)
:pimpl(new Impl)
,config(cfg) 
{
	pimpl->map = flatworld::load_scene(map_url);
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

    pimpl->pf = pfcpp::ParticleFilter<flatworld::Pose>(states, {{0.4,0.5,0.05,0.1,0.0,0.0}});	
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
	double max_range = config.sensor_settings.ray_length;
	pfcpp::BeamSensorModel sensor_model(config.sensor_settings);
	pfcpp::VelocityMotionModelSampler movement;
	pimpl->robot_pos = movement(pimpl->robot_pos, pimpl->path[pimpl->step]);

    pimpl->pf([&](auto p){ return sensor_model(flatworld::measurement(pimpl->robot_pos, pimpl->map, config.beams_count, max_range),
                                     		   flatworld::measurement(p, pimpl->map, config.beams_count, max_range));},
    		   pimpl->path[pimpl->step]);

    pimpl->step++;		
}

QPointF to_q_point(const ShapesMap::Point& x)
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
*/