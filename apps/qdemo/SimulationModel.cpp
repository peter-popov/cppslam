#include "SimulationModel.hpp"

#include <map/shapes_map.hpp>
#include <pfcpp/mcl.hpp>
#include <simulations/flatworld.hpp>
#include <pfcpp/sensor.hpp>
#include <pfcpp/motion.hpp>

using namespace pfcpp::maps;

namespace pfcpp
{


template<>
double get_v(const VelocityControl& p)
{
	return p.v();
}

template<>
double get_w(const VelocityControl& p)
{
	return p.w();
}

template<>
double get_t(const VelocityControl& p)
{
	return p.t();
}

}

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
: m_currentPose(new Pose())
{

}

Simulation::~Simulation()
{
}
	

QQmlListProperty<Particle> Simulation::particles()
{
	return QQmlListProperty<Particle>(this, &m_particles, list_count<Particle>, list_at<Particle>);
}

QQmlListProperty<PointW> Simulation::sensorBeams()
{
	return QQmlListProperty<PointW>(this, &m_sensorBeams, list_count<PointW>, list_at<PointW>);
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

void Simulation::move(VelocityControl* control)
{
	
	pfcpp::BeamSensorModel sensor_model({m_sensorModel->maxRange(), {m_sensorModel->a0(), m_sensorModel->a1(), m_sensorModel->a2(), m_sensorModel->a3()}, m_sensorModel->sigma(), m_sensorModel->lambda()});
	pfcpp::VelocityMotionModelSampler movement;
	pimpl->robot_pos = movement(pimpl->robot_pos, *control);

	auto actual_measument = flatworld::measurement(pimpl->robot_pos, pimpl->map, m_mcl->numberOfBeams(), m_sensorModel->maxRange());
	auto expected_measument = [&](auto p){ return flatworld::measurement(p, 
		pimpl->map, m_mcl->numberOfBeams(), m_sensorModel->maxRange()); };


    pimpl->pf([&](auto p){ return sensor_model(actual_measument, expected_measument(p));},
    		  *control);

	std::vector<ShapesMap::Position> sense_points;
    std::tie(std::ignore, sense_points) = flatworld::measurement_with_coords(pimpl->robot_pos, pimpl->map, m_mcl->numberOfBeams(), m_sensorModel->maxRange());
 	m_sensorBeams.clear();
	for (auto p: sense_points)
	{
		m_sensorBeams.push_back(std::make_shared<PointW>(QPointF{std::get<0>(p), std::get<1>(p)}));
	}

	m_currentPose->setPosition({pimpl->robot_pos.x, pimpl->robot_pos.y});


	updateParticlesList();
    emit updated();
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