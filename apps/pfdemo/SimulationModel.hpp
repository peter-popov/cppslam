#ifndef SIMULATION_MODEL_H
#define SIMULATION_MODEL_H

#include <QtWidgets>
#include <memory>



struct SimulationConfig
{
	SimulationConfig(size_t particles, size_t beams):particle_count(particles),
		beams_count(beams)
	{		
	}

	size_t particle_count;
	size_t beams_count;

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


#endif //SIMULATION_MODEL_H
