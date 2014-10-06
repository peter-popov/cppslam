#pragma once
#include <QtWidgets>
#include <pfcpp/sensor.hpp>

class DistributionPlot : public QWidget
{
    Q_OBJECT

public:
    
    DistributionPlot(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public:
    void setShape(std::vector<double>&& values, double m);
    void setPen(const QPen &pen);

protected:
    void paintEvent(QPaintEvent *event);

private:
    std::vector<double> values;
    double max_value; 
    QPen pen;
};


class SensorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SensorWindow(QWidget *parent = 0);
    ~SensorWindow();

    pfcpp::SensorSetings getSettings() { return settings;}
private slots:
    void on_value_changed();

private:
    DistributionPlot* plot;    

    QLineEdit* edit_num_rays;
	QLineEdit* edit_a0, *edit_a1, *edit_a2, *edit_a3;
	QLineEdit* sigma, *lambda;
   	pfcpp::SensorSetings settings;

};
