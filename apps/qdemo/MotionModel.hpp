#pragma once

#include <QtWidgets>
#include <QtQml>
#include "SimulationModel.hpp"

class MotionSample : public QObject
{
	Q_OBJECT

	Q_PROPERTY(Pose* startPose READ startPose)
	Q_PROPERTY(QQmlListProperty<Pose> samples READ samples)
	Q_PROPERTY(QQmlListProperty<Pose> moves READ moves)

	struct Control
	{
		double v;
		double w;
		double time = 1.0;
	};
public:
	MotionSample();

	MotionSample(QPointF pos, double dir);

	Pose* startPose() const {return m_startPose.get();}
	QQmlListProperty<Pose> samples();
	QQmlListProperty<Pose> moves();
	
	void recalculate(Control ctrl, std::array<double, 6> params);

private:
	std::unique_ptr<Pose> m_startPose;
	QList<std::shared_ptr<Pose>> m_samples;
	QList<std::shared_ptr<Pose>> m_moves;
};


class MotionModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(MotionSample* straightMotion READ straightMotion)
	Q_PROPERTY(MotionSample* rotationMotion READ rotationMotion)
	
	Q_PROPERTY(double a0 READ a0 WRITE setA0 NOTIFY modelChanged)
	Q_PROPERTY(double a1 READ a1 WRITE setA1 NOTIFY modelChanged)
	Q_PROPERTY(double a2 READ a2 WRITE setA2 NOTIFY modelChanged)
	Q_PROPERTY(double a3 READ a3 WRITE setA3 NOTIFY modelChanged)


signals:
	void modelChanged();

public:
	MotionModel();

	MotionSample* straightMotion() { return &m_straightMotion; }
	MotionSample* rotationMotion() { return &m_rotationMotion; }

	std::array<double, 6> params();

	double a0() const { return m_a0; }
	double a1() const { return m_a1; }
	double a2() const { return m_a2; }
	double a3() const { return m_a3; }

	void setA0(double v) { if (m_a0 == v) return; m_a0 = v; recalculate();}
	void setA1(double v) { if (m_a1 == v) return; m_a1 = v; recalculate();}
	void setA2(double v) { if (m_a2 == v) return; m_a2 = v; recalculate();}
	void setA3(double v) { if (m_a3 == v) return; m_a3 = v; recalculate();}

private:
	void recalculate();

private:
	double m_a0;
	double m_a1;
	double m_a2;
	double m_a3;

	MotionSample m_straightMotion;
	MotionSample m_rotationMotion;
};