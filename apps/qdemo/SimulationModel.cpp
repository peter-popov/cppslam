#include "SimulationModel.hpp"

#include <map/shapes_map.hpp>
#include <map/flatworld.hpp>
#include <pfcpp/mcl.hpp>
#include <pfcpp/sensor.hpp>
#include <pfcpp/motion.hpp>

#include "MotionModel.hpp"

namespace flatworld = pfcpp::maps;

namespace pfcpp
{


template<>
double get_velocity(const VelocityControl& p)
{
	return p.v();
}

template<>
double get_rotation(const VelocityControl& p)
{
	return p.w();
}

template<>
double get_time(const VelocityControl& p)
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
	qp.setOrientation(p.heading);
	return qp;
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
	pimpl->robot_pos.heading = startPosition.orientation();
    
    std::vector<flatworld::Pose> states;
    for (int i = 0; states.size() < m_mcl->numberOfParticles(); ++i)
    {
        for (auto p: flatworld::random_pose_with_direction(pimpl->map))
        {
	        if (pimpl->map.is_occupied(std::make_tuple(p.x,p.y)))
	            continue;
	        states.push_back(p);
	    }
    }

    pimpl->pf = pfcpp::ParticleFilter<flatworld::Pose>(states);

    updateParticlesList();
    emit initialized();
}


void Simulation::move(VelocityControl* control)
{
    if (!m_motionModel || !m_sensorModel)
    {
    	return;
    }

	pfcpp::OdometryMotionModelSampler motion_model(m_motionModel->params());

	pfcpp::BeamSensorModel sensor_model({m_sensorModel->maxRange(), {m_sensorModel->a0(), m_sensorModel->a1(), m_sensorModel->a2(), m_sensorModel->a3()}, m_sensorModel->sigma(), m_sensorModel->lambda()});
	pfcpp::VelocityMotionModelSampler movement;
	pimpl->robot_pos = movement(pimpl->robot_pos, *control);

	std::vector<double> actual_measument;
	std::vector<pfcpp::maps::ShapesMap::Position> sense_points;
    std::tie(actual_measument, sense_points) = flatworld::measurement_with_coords(pimpl->robot_pos, pimpl->map, m_mcl->numberOfBeams(), m_sensorModel->maxRange());
	auto expected_measument = [&](auto p){ return flatworld::measurement(p, 
		pimpl->map, m_mcl->numberOfBeams(), m_sensorModel->maxRange()); };

    pimpl->pf([&](auto p){ return sensor_model(actual_measument, expected_measument(p)); },
    		  [&](auto p){ return motion_model(p, movement(p, *control)); });
	
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