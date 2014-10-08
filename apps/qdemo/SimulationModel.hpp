#pragma once

#include <QtWidgets>
#include <QtQml>
#include <memory>
#include <pfcpp/sensor.hpp>
#include "SensorSettingsModel.hpp"



class MclSettings : public QObject
{
	Q_OBJECT

	Q_ENUMS(ResamplingType)

	Q_PROPERTY(uint32_t numberOfParticles READ numberOfParticles WRITE setNumberOfParticles)	
	Q_PROPERTY(uint32_t numberOfBeams READ numberOfParticles WRITE setNumberOfParticles)	
	Q_PROPERTY(ResamplingType resamplingType READ resamplingType WRITE setResamplingType)	
public:
	enum ResamplingType
	{
		Multimodal, Stratified, Systematic
	};
    //...
};


class Pose
{
	Q_OBJECT
	Q_PROPERTY(QPointF position READ position)
	Q_PROPERTY(double orientation READ orientation)
};

class Particle
{
	Q_OBJECT
	Q_PROPERTY(Pose* pose READ pose)
	Q_PROPERTY(double weight READ weight)
};

class VelocityControl
{
	Q_OBJECT
	Q_PROPERTY(double v READ v)
	Q_PROPERTY(double w READ w)
	Q_PROPERTY(double t READ t)
};

class OdometryControl
{
	Q_OBJECT
	Q_PROPERTY(double movement READ movement)
	Q_PROPERTY(double startAngle READ startAngle)
	Q_PROPERTY(double endAngle READ endAngle)
};


//Think about the polymorphic Control type!!!
class Simulation: QObject
{
	Q_OBJECT
	
	Q_PROPERTY(MclSettings* mcl READ mcl WRITE setMcl NOTIFY mclChanged)	
	Q_PROPERTY(SensorSettingsModel* sensorModel READ sensorModel WRITE setSensorModel NOTIFY sensorModelChamged)
	//Q_PROPERTY(MotionSetingsModel motionModel READ motionModel WRITE setMotionModel)

	Q_PROPERTY(QQmlListProperty<Particle> particles READ particles)	
	Q_PROPERTY(QQmlListProperty<QPointF> sensorBeams READ sensorBeams)
	Q_PROPERTY(Pose* currentPose READ currentPose)
			
public slots:
	void init();

signals:
	void initialized();
	void updated();
	void mclChanged(MclSettings*);
	void sensorModelChamged(SensorSettingsModel*);
	void pathChanged(const QQmlListProperty<QPointF>& path);

};



struct SimulationConfig
{
	SimulationConfig(size_t particles = 1, size_t beams = 1):particle_count(particles),
		beams_count(beams)
	{		
	}

	size_t particle_count;
	size_t beams_count;
	pfcpp::SensorSetings sensor_settings;

	enum
	{
		VELOCITY,
		ODOMETRY
	} motion_model;

	enum 
	{
		MULTIMODAL, STRATIFIED, SYSTEMATIC
	} resampler;


	enum 
	{
		BEAM_RAY_TRACING, BEAM_END_POINT_MAP		
	} sensor_model;

	enum
	{
		MULTIPLICATION, LOG_SUM
	} beam_combination;
};

struct Control
{
    double v;
    double w;    
};

class SimulationModel
{
public:
	SimulationModel( SimulationConfig config, std::tuple<double, double, double> start_pos, std::vector<Control> path, std::string map_url );

	~SimulationModel();
	std::tuple<QPointF, double> current_position() const;
	std::vector<QPointF> measurment_end_points() const;

	size_t particle_count( ) const;
	std::tuple<QPointF, double> particle(size_t i) const;

	std::vector<QPolygonF> export_map();

	void update();

private:
	struct Impl;
	std::unique_ptr<Impl> pimpl;
	SimulationConfig config;
};
