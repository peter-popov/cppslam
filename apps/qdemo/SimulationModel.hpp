#pragma once

#include <QtWidgets>
#include <QtQml>
#include <memory>
#include <pfcpp/sensor.hpp>
#include "SensorSettingsModel.hpp"

class Simulation;

class MclSettings : public QObject
{
	Q_OBJECT

	Q_ENUMS(ResamplingType)

	Q_PROPERTY(int numberOfParticles READ numberOfParticles WRITE setNumberOfParticles)	
	Q_PROPERTY(int numberOfBeams READ numberOfBeams WRITE setNumberOfBeams)	
	Q_PROPERTY(ResamplingType resamplingType READ resamplingType WRITE setResamplingType)	
public:
	enum ResamplingType
	{
		Multimodal, Stratified, Systematic
	};
    
    int numberOfParticles() const {return m_numberOfParticles;}
  	int numberOfBeams() const {return m_numberOfBeams;}
    ResamplingType resamplingType() const {return m_resamplingType;}
      
public slots:
	void setNumberOfParticles(int v) {m_numberOfParticles = v;}
	void setNumberOfBeams(int v) {m_numberOfBeams = v;}
	void setResamplingType(ResamplingType v) {m_resamplingType = v;}

private:
	int m_numberOfParticles;
	int m_numberOfBeams;
	ResamplingType m_resamplingType;
};


class Pose: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QPointF position READ position WRITE setPosition)
	Q_PROPERTY(double orientation READ orientation WRITE setOrientation)
public:	
	Pose(QPointF p, double o): m_position(p), m_orientation(o) {}
	Pose(): Pose({0, 0}, 0.0) {}
	Pose(const Pose& other) : Pose(other.position(), other.orientation()) {}
	const Pose& operator=(const Pose& other) 
	{ 
		m_position = other.position();
		m_orientation = other.orientation();
		return *this;
	}

	QPointF position() const {return m_position;}
	double orientation() const {return m_orientation;}

public slots:
	void setPosition(QPointF v) {m_position = v;}
	void setOrientation(double v) {m_orientation = v;}

private:
	QPointF m_position;
	double m_orientation;
};


class Particle: public QObject
{
	Q_OBJECT
	Q_PROPERTY(Pose* pose READ pose)
	Q_PROPERTY(double weight READ weight)

public:
	Pose* pose() {return &m_pose;} 
	double weight() const {return m_weight;} 

public slots:
	void setpose(Pose v) {m_pose = v;}
	void setweight(double v) {m_weight = v;}
private:
	Pose m_pose;
	double m_weight;
};


class VelocityControl: public QObject
{
	Q_OBJECT
	Q_PROPERTY(double v READ v WRITE setV)
	Q_PROPERTY(double w READ w WRITE setW)
	Q_PROPERTY(double t READ t WRITE setT)
public:
	double v() const {return m_v;}
	double w() const {return m_w;}
	double t() const {return m_t;}

public slots:
	void setV(double v) {m_v = v;}
	void setW(double v) {m_w = v;}
	void setT(double v) {m_t = v;}

private:
	double m_v;
	double m_w;
	double m_t;
};


class Simulation: public QObject
{
	Q_OBJECT	
	Q_PROPERTY(MclSettings* mcl READ mcl WRITE setMcl NOTIFY mclChanged)	
	Q_PROPERTY(SensorSettingsModel* sensorModel READ sensorModel WRITE setSensorModel NOTIFY sensorModelChamged)


	Q_PROPERTY(QQmlListProperty<Particle> particles READ particles)	
	Q_PROPERTY(QQmlListProperty<QPointF> sensorBeams READ sensorBeams)
	Q_PROPERTY(Pose currentPose READ currentPose)
			
public:
	Simulation();
	~Simulation();
	
	MclSettings* mcl() const {return m_mcl;}
	SensorSettingsModel* sensorModel() const {return m_sensorModel;}
	Pose currentPose() const {return m_currentPose;}
	QQmlListProperty<Particle> particles();
	QQmlListProperty<QPointF> sensorBeams(); 

public slots:
	void init(QString mapUrl, Pose* startPosition);
	void move(const VelocityControl& control);
	void setMcl(MclSettings* v) {m_mcl = v; emit mclChanged(v);}
	void setSensorModel(SensorSettingsModel* v) {m_sensorModel = v; emit sensorModelChamged(v);}

signals:
	void initialized();
	void updated();
	void mclChanged(MclSettings*);
	void sensorModelChamged(SensorSettingsModel*);

private:
	void updateParticlesList();

private:
	MclSettings* m_mcl;
	SensorSettingsModel* m_sensorModel;
	Pose m_currentPose;	
	QList<std::shared_ptr<Particle>> m_particles;
	QList<std::shared_ptr<QPointF>> m_sensorBeams;
	
	struct Impl;
	std::unique_ptr<Impl> pimpl;
};


/*
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
*/