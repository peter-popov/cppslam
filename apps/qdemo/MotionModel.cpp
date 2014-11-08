#include "MotionModel.hpp"
#include <pfcpp/motion.hpp>

double get_x(const Pose& p)
{
	return p.position().x();
}

double get_y(const Pose& p)
{
	return p.position().y();
}

double get_heading(const Pose& p)
{
	return p.orientation();
}


MotionSample::MotionSample()
: m_startPose(new Pose({5,5}, 0.0))
{

}

MotionSample::MotionSample(QPointF pos, double dir)
: m_startPose(new Pose(pos, dir))
{
}

QQmlListProperty<Pose> MotionSample::samples()
{
	return QQmlListProperty<Pose>(this, &m_samples, list_count<Pose>, list_at<Pose>);
}

QQmlListProperty<Pose> MotionSample::moves()
{
	return QQmlListProperty<Pose>(this, &m_moves, list_count<Pose>, list_at<Pose>);
}

void MotionSample::recalculate(Pose end_pose, std::array<double, 4> params)
{
	//pfcpp::VelocityMotionModelSampler sampler(std::move(params));
	pfcpp::OdometryMotionModelSampler sampler(params);

	m_moves.clear();
	m_moves.push_back(std::make_shared<Pose>(*m_startPose));
	m_moves.push_back(std::make_shared<Pose>(end_pose));

	m_samples.clear();
	for (int i = 0; i < 400; ++i)
	{
		auto p = sampler(*m_startPose, end_pose);
		m_samples.push_back(std::make_shared<Pose>(p));
	}
}


MotionModel::MotionModel()
: m_rotationMotion({5, 5}, 0.0)
, m_straightMotion({5, 5}, 1.57079632679)
{
}

std::array<double, 4> MotionModel::params()
{
	return {m_a0, 
			m_a1, 
			m_a2, 
			m_a3};
}

void MotionModel::recalculate()
{
	m_rotationMotion.recalculate({{25, 25}, 1.57079632679}, params());
	m_straightMotion.recalculate({{5, 30}, 1.57079632679}, params());
	
	emit modelChanged();
}