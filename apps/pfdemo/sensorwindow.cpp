#include "sensorwindow.hpp"
#include <pfcpp/sensor.hpp>


DistributionPlot::DistributionPlot(QWidget *parent)
{

}

QSize DistributionPlot::minimumSizeHint() const
{
	return QSize(200, 100);
}

QSize DistributionPlot::sizeHint() const
{
	return QSize(400, 200);
}

void DistributionPlot::setShape(std::vector<double>&& v, double m)
{
	values.swap(v);
	max_value = m;	
	update();
}

void DistributionPlot::setPen(const QPen &p)
{
	pen = p;
}

void DistributionPlot::paintEvent(QPaintEvent *event)
{
	if (values.empty())
		return;

	QPainter painter(this);
    painter.setPen(pen);
   	painter.setRenderHint(QPainter::Antialiasing, true);

   	auto max_y = *std::max_element(values.begin(), values.end());
	
	auto scale_y = (this->size().height() - 30) / max_y;

   	QPainterPath path;
    path.moveTo(0, values[0] * scale_y);
    for (size_t i = 0; i < values.size(); ++i)
    {
    	auto x = i * this->size().width() / values.size();
    	auto y = this->size().height() - values[i] * scale_y;
    	path.lineTo(x, y);	    	
    }
    
    painter.drawPath(path);
}



SensorWindow::SensorWindow(QWidget *parent)
{
	plot = new DistributionPlot;

	auto settings = new QWidget;
	auto settings_layout = new QVBoxLayout;
	settings->setLayout(settings_layout);
	edit_num_rays = new QLineEdit("200");
	edit_a0 = new QLineEdit("0.25");
	edit_a1 = new QLineEdit("0.25");
	edit_a2 = new QLineEdit("0.25");
	edit_a3 = new QLineEdit("0.25");
	sigma = new QLineEdit("16");
	lambda = new QLineEdit("0.0");

	settings_layout->addWidget(edit_num_rays);
	settings_layout->addWidget(edit_a0);
	settings_layout->addWidget(edit_a1);
	settings_layout->addWidget(edit_a2);
	settings_layout->addWidget(edit_a3);
	settings_layout->addWidget(sigma);
	settings_layout->addWidget(lambda);
	connect(edit_num_rays, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));
	connect(edit_a0, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));
	connect(edit_a1, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));
	connect(edit_a2, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));
	connect(edit_a3, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));
	connect(sigma, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));
	connect(lambda, SIGNAL(editingFinished()), this, SLOT(on_value_changed()));

	auto layout = new QHBoxLayout;
	layout->addWidget(settings);
	layout->addWidget(plot);
	setLayout(layout);
	setWindowTitle("Sensow setting");
}


void SensorWindow::on_value_changed()
{
	settings.ray_length = edit_num_rays->text().toDouble();
	settings.sigma = sigma->text().toDouble();
	settings.lambda = lambda->text().toDouble();
	settings.a[0] = edit_a0->text().toDouble();
	settings.a[1] = edit_a1->text().toDouble();
	settings.a[2] = edit_a2->text().toDouble();
	settings.a[3] = edit_a3->text().toDouble();


	pfcpp::BeamSensorModel sensor_model(settings);
	std::vector<double> values;
	for (int i = 0; i < settings.ray_length; ++i)
	{
		std::vector<double> mes{i*1.0};
		std::vector<double> expected{100};
		
		values.push_back(sensor_model(mes, expected));
	}
	plot->setShape(std::move(values), settings.ray_length);
}

SensorWindow::~SensorWindow()
{

}

