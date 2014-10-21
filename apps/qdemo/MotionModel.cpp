#include "MotionModel.hpp"
#include <pfcpp/motion.hpp>

namespace pfcpp
{

template<>
double get_x(const Pose& p)
{
	return p.position().x();
}

template<>
double get_y(const Pose& p)
{
	return p.position().y();
}

template<>
double get_dir(const Pose& p)
{
	return p.orientation();
}

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

void MotionSample::recalculate(Control ctrl, std::array<double, 6> params)
{
	pfcpp::VelocityMotionModelSampler sampler(std::move(params));

	m_samples.clear();
	for (int i = 0; i < 400; ++i)
	{
		auto p = sampler(*m_startPose, ctrl);
		m_samples.push_back(std::make_shared<Pose>(p));
	}

	m_moves.clear();
	pfcpp::VelocityMotionModelSampler exact;
	for (int i = 0; i <= 5; ++i)
	{
		auto p = exact(*m_startPose, Control{ctrl.v*i/5.0, ctrl.w*i/5.0});
		m_moves.push_back(std::make_shared<Pose>(p));
	}
}


MotionModel::MotionModel()
: m_rotationMotion({5, 5}, 0.0)
, m_straightMotion({5, 5}, 1.57079632679)
{
}

std::array<double, 6> MotionModel::params()
{
	return {m_a0 * (1 - m_a1), 
			m_a0 * m_a1, 
			m_a2 * (1 - m_a3), 
			m_a2 * m_a3, 
			0.0, 
			0.0};
}

void MotionModel::recalculate()
{
	m_straightMotion.recalculate({30, 0}, params());
	m_rotationMotion.recalculate({30, 1.57079632679}, params());

	emit modelChanged();
}