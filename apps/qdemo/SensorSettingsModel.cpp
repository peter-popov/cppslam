#include "SensorSettingsModel.hpp"	
#include <pfcpp/sensor.hpp>

const QList<double>& SensorSettingsModel::samples() const
{
	return m_samples;
}

void SensorSettingsModel::recalculate()
{
	pfcpp::BeamSensorModel model( {m_maxRange, {m_a0, m_a1, m_a2, m_a3}, m_sigma, m_lambda});

	m_samples.clear();
	auto delta = m_maxRange/100.0;
	auto maxValue = 0.0;

	for (double x = 0; x < m_maxRange; x += delta)
	{
		auto p = model(x, 0.5*m_maxRange);
		m_samples.push_back(p);
		if (p > maxValue ) maxValue = p;
	}

	if (maxValue < 0.00000001) 
		maxValue = 1;
	else 
	{
		for ( int i = 0; i < m_samples.size(); ++i)
		{
			m_samples[i] /= maxValue;
		}
	}


	//std::cout << "recalculated " << std::endl;
	emit modelChanged();
}
